import unreal
from PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    PAL_NAME,
    GLOBAL_AS_PREFIX,
    GLOBAL_BS_PREFIX,
    PAL_AS_PREFIX,
    PAL_BS_PREFIX,
)
# ------------------------------------------------------------
# 설정
# ------------------------------------------------------------

#GLOBAL_ANIM_DIR   = "/Game/Pal/Model/Global/Animation"
#MONSTER_ROOT      = "/Game/Pal/Model/Monster"
#PAL_NAME = "Anubis"
TEMPLATE_ABP_NAME = "ABP_Global_Template"
TEMPLATE_ABP_PATH = f"{GLOBAL_ANIM_DIR}/{TEMPLATE_ABP_NAME}.{TEMPLATE_ABP_NAME}"

# Global 에서 쓰는 이름 규칙
#GLOBAL_AS_PREFIX  = "AS_MM_"        # AnimSequence
#GLOBAL_BS_PREFIX  = "BS_MM_"    # BlendSpace

# Pal 에서 사용할 이름 규칙
#PAL_AS_PREFIX     = "AS_"           # AS_<PalName>_<Action>
#PAL_BS_PREFIX     = "BS_"           # BS_<PalName>_<Action>

editor_lib  = unreal.EditorAssetLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
registry    = unreal.AssetRegistryHelpers.get_asset_registry()


# ------------------------------------------------------------
# 공통 유틸
# ------------------------------------------------------------

def load_template_abp():
    """Global_Animation 폴더의 ABP_Global_Template 로드"""
    abp = editor_lib.load_asset(TEMPLATE_ABP_PATH)
    if not abp:
        unreal.log_error(f"[오류] 템플릿 ABP 로드 실패: {TEMPLATE_ABP_PATH}")
        return None

    if not isinstance(abp, unreal.AnimBlueprint):
        unreal.log_error(f"[오류] 템플릿이 AnimBlueprint가 아님: {TEMPLATE_ABP_PATH}")
        return None

    return abp


def find_pal_skeleton(pal_folder):
    """각 팰 폴더의 Mesh 하위에서 SkeletalMesh 하나 찾아 Skeleton 반환"""
    mesh_dir = pal_folder + "/Mesh"

    if not editor_lib.does_directory_exist(mesh_dir):
        unreal.log_warning(f"[SKIP] Mesh 폴더 없음: {mesh_dir}")
        return None

    asset_datas = registry.get_assets_by_path(mesh_dir, recursive=True)
    for ad in asset_datas:
        if ad.asset_class_path.asset_name != "SkeletalMesh":
            continue

        mesh = ad.get_asset()
        if not mesh:
            continue

        skel = mesh.get_editor_property("skeleton")
        if skel:
            return skel

    unreal.log_warning(f"[SKIP] Skeleton 없음: {mesh_dir}")
    return None


def collect_template_assets_from_abp(template_abp):
    """
    템플릿 ABP 안에서 '실제로 사용 중인' AnimationAsset만 수집.
    - AnimGraphNode_SequencePlayer → AnimSequence (AS_MM_*)
    - AnimGraphNode_BlendSpacePlayer → BlendSpace (BS_Global_*)
    """
    seq_map = {}  # action -> AnimSequence (Global)
    bs_map  = {}  # action -> BlendSpace (Global)

    # SequencePlayer 노드들
    seq_nodes = template_abp.get_nodes_of_class(unreal.AnimGraphNode_SequencePlayer, True)
    for graph_node in seq_nodes:
        internal_node = graph_node.get_editor_property("node")  # FAnimNode_SequencePlayer
        anim = internal_node.get_editor_property("sequence")
        if not anim:
            continue

        name = anim.get_name()
        if not name.startswith(GLOBAL_AS_PREFIX):
            continue

        action = name[len(GLOBAL_AS_PREFIX):]  # "AS_MM_Walk" → "Walk"
        seq_map[action] = anim

    # BlendSpacePlayer 노드들
    bs_nodes = template_abp.get_nodes_of_class(unreal.AnimGraphNode_BlendSpacePlayer, True)
    for graph_node in bs_nodes:
        internal_node = graph_node.get_editor_property("node")  # FAnimNode_BlendSpacePlayer
        blend_space = internal_node.get_editor_property("blend_space")
        if not blend_space:
            continue

        name = blend_space.get_name()
        if not name.startswith(GLOBAL_BS_PREFIX):
            continue

        action = name[len(GLOBAL_BS_PREFIX):]  # "BS_Global_Walk" → "Walk"
        bs_map[action] = blend_space

    unreal.log(f"[INFO] Template ABP 기반 AnimSequence 개수: {len(seq_map)}")
    unreal.log(f"[INFO] Template ABP 기반 BlendSpace 개수: {len(bs_map)}")
    return seq_map, bs_map


# ------------------------------------------------------------
# 1. 템플릿으로부터 자식 AnimBP 생성
# ------------------------------------------------------------

def create_or_load_pal_abp(template_abp, pal_folder, pal_name):
    """
    1. Global_Animation 의 ABP_Global_Template 로부터 각 팰용 자식 AnimBlueprint 생성 (또는 기존 것 로드)
       1-1. Skeleton : 각 팰 폴더의 Mesh/SK_팰이름_Skeleton 으로부터 가져온 Skeleton
       1-2. AnimBP 이름 : ABP_팰이름
            위치      : 각 팰 폴더 루트 (pal_folder)
       1-3. 이미 ABP_팰이름 이 있으면 생성 스킵, 오버라이드만 진행
    """
    pal_abp_name = f"ABP_{pal_name}"
    pal_abp_path = f"{pal_folder}/{pal_abp_name}.{pal_abp_name}"

    # 1-3) 이미 있으면 생성 스킵, 로드만
    if editor_lib.does_asset_exist(pal_abp_path):
        unreal.log_warning(f"[SKIP] 이미 AnimBP 존재, 오버라이드만 수행: {pal_abp_path}")
        pal_abp = editor_lib.load_asset(pal_abp_path)
        if not isinstance(pal_abp, unreal.AnimBlueprint):
            unreal.log_error(f"[오류] 기존 에셋이 AnimBlueprint가 아님: {pal_abp_path}")
            return None
        return pal_abp

    # Skeleton 찾기
    skel = find_pal_skeleton(pal_folder)
    if not skel:
        return None

    # 템플릿 ABP의 GeneratedClass 를 부모로 사용

    parent_class = template_abp.generated_class()
    if not parent_class:
        unreal.log_error(f"[오류] 템플릿 ABP의 generated_class 조회 실패: {TEMPLATE_ABP_PATH}")
        return None

    factory = unreal.AnimBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    factory.set_editor_property("target_skeleton", skel)

    pal_abp = asset_tools.create_asset(
        asset_name   = pal_abp_name,
        package_path = pal_folder,   # 각 팰 폴더 루트
        asset_class  = unreal.AnimBlueprint,
        factory      = factory
    )

    if not pal_abp:
        unreal.log_error(f"[오류] Pal AnimBP 생성 실패: {pal_abp_name}")
        return None

    unreal.log(f"  [생성] {pal_abp_path}")
    return pal_abp


# ------------------------------------------------------------
# 2. Asset Override 설정
# ------------------------------------------------------------

def apply_overrides_for_pal(pal_abp,
                            pal_folder,
                            pal_name,
                            global_seq_map,
                            global_bs_map):
    """
    2. 자식 AnimBP(ABP_팰이름)의 에셋 오버라이드 설정.

       - 기준: ABP_Global_Template 에서 실제로 사용중인 AnimationAsset
         (collect_template_assets_from_abp 에서 수집한 global_seq_map / global_bs_map)

       2-1. 이름 규칙에 맞게 교체:
            AS_MM_Start   -> AS_팰이름_Start
            BS_Global_Run -> BS_팰이름_Run

       2-2. 팰 쪽 Animation / BlendSpace 가 없으면
            해당 타깃에 대해 override 를 None 으로 설정(= NULL 오버라이드)
    """
    pal_anim_dir = f"{pal_folder}/Animation"

    # 2-1) AnimSequence 오버라이드
    for action, global_anim in global_seq_map.items():
        pal_anim_name = f"{PAL_AS_PREFIX}{pal_name}_{action}"
        pal_anim_path = f"{pal_anim_dir}/{pal_anim_name}.{pal_anim_name}"

        if editor_lib.does_asset_exist(pal_anim_path):
            override_asset = editor_lib.load_asset(pal_anim_path)
            unreal.log(f"  [OVERRIDE] {global_anim.get_name()} → {pal_anim_name}")
        else:
            override_asset = global_anim
            unreal.log_warning(f"  [OVERRIDE] {global_anim.get_name()} → NULL (Pal Anim 없음: {pal_anim_path})")

        # AnimBlueprint 인스턴스 메서드 사용
        pal_abp.add_node_asset_override(global_anim, override_asset, False)

    # 2-1) BlendSpace 오버라이드
    for action, global_bs in global_bs_map.items():
        pal_bs_name = f"{PAL_BS_PREFIX}{pal_name}_{action}"
        pal_bs_path = f"{pal_anim_dir}/{pal_bs_name}.{pal_bs_name}"

        if editor_lib.does_asset_exist(pal_bs_path):
            override_bs = editor_lib.load_asset(pal_bs_path)
            unreal.log(f"  [OVERRIDE] {global_bs.get_name()} → {pal_bs_name}")
        else:
            override_bs = global_bs
            unreal.log_warning(f"  [OVERRIDE] {global_bs.get_name()} → NULL (Pal BS 없음: {pal_bs_path})")
        
        pal_abp.add_node_asset_override(global_bs, override_bs, False)

    # 저장
    unreal.BlueprintEditorLibrary.compile_blueprint(pal_abp)
    editor_lib.save_asset(pal_abp.get_path_name())
    

# ------------------------------------------------------------
# main
# ------------------------------------------------------------

def main():
    unreal.log("==============================================")
    unreal.log("[START] ABP_Global_Template → Pal AnimBP 생성 + 오버라이드")
    unreal.log("==============================================")

    # 템플릿 ABP 로드
    template_abp = load_template_abp()
    if not template_abp:
        return

    # 템플릿 ABP 안에서 실제 사용 중인 Global 애니만 수집
    global_seq_map, global_bs_map = collect_template_assets_from_abp(template_abp)

    # Monster 루트 하위의 각 팰 폴더 처리
    #pal_folders = registry.get_sub_paths(MONSTER_ROOT, recurse=False)

    pal_folder = MONSTER_ROOT + "/" + PAL_NAME
    #for pal_folder in pal_folders:
    pal_name = pal_folder.split("/")[-1]
    unreal.log(f"------ {pal_name} 처리 시작 ------")

    # 1. 자식 AnimBP 생성 또는 로드
    pal_abp = create_or_load_pal_abp(template_abp, pal_folder, pal_name)
    if not pal_abp:
        unreal.log_warning(f"[SKIP] AnimBP 생성/로드 실패: {pal_name}")

    else:
        # 2. 에셋 오버라이드 적용
        apply_overrides_for_pal(
            pal_abp       = pal_abp,
            pal_folder    = pal_folder,
            pal_name      = pal_name,
            global_seq_map= global_seq_map,
            global_bs_map = global_bs_map
        )

        unreal.log(f"------ {pal_name} 처리 종료 ------")


    unreal.log("==============================================")
    unreal.log("[완료] AnimBP 생성 및 에셋 오버라이드 전체 종료")
    unreal.log("==============================================")


if __name__ == "__main__":
    main()
