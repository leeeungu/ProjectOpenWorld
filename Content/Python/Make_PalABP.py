import unreal
from PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    GLOBAL_AS_PREFIX,
    GLOBAL_BS_PREFIX,
    PAL_AS_PREFIX,
    PAL_BS_PREFIX,
    GLOBAL_DIR,
    find_asset,
)

# ============================================================
# 설정
#  - TARGET_ABP_PATH : 기준이 되는 템플릿 AnimBlueprint
#  - PAL_NAME        : PalConfig 에서 가져옴
#  - 결과 ABP 이름   : ABP_<PAL_NAME>
#  - 결과 위치       : /Game/Pal/Model/Monster/<PAL_NAME> (pal_folder 루트)
# ============================================================
PAL_NAME = "PinkCat" 
TEMPLATE_ABP_NAME = "_Monster"
TARGET_ABP_PATH   = f"{GLOBAL_DIR}/ABP_MM{TEMPLATE_ABP_NAME}Template"

editor_lib  = unreal.EditorAssetLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
registry    = unreal.AssetRegistryHelpers.get_asset_registry()

# ------------------------------------------------------------
# 템플릿 ABP 안에서 실제 사용 중인 애니메이션 수집
#   - SequencePlayer → AnimSequence (AS_MM_*)
#   - BlendSpacePlayer → BlendSpace (BS_MM_*)
# ------------------------------------------------------------
def collect_template_assets_from_abp(
    template_abp: unreal.AnimBlueprint,
) -> tuple[dict[str, unreal.AnimSequence], dict[str, unreal.BlendSpace]]:

    seq_map: dict[str, unreal.AnimSequence] = {}
    bs_map: dict[str, unreal.BlendSpace] = {}

    # SequencePlayer 노드들
    seq_nodes = template_abp.get_nodes_of_class(
        unreal.AnimGraphNode_SequencePlayer, True
    )
    for graph_node in seq_nodes:
        internal_node = graph_node.get_editor_property("node")  # FAnimNode_SequencePlayer
        anim = internal_node.get_editor_property("sequence")
        if not anim:
            continue

        name = anim.get_name()
        if not name.startswith(GLOBAL_AS_PREFIX):
            continue

        action = name[len(GLOBAL_AS_PREFIX) :]  # "AS_MM_Walk" → "Walk"
        seq_map[action] = anim

    # BlendSpacePlayer 노드들
    bs_nodes = template_abp.get_nodes_of_class(
        unreal.AnimGraphNode_BlendSpacePlayer, True
    )
    for graph_node in bs_nodes:
        internal_node = graph_node.get_editor_property(
            "node"
        )  # FAnimNode_BlendSpacePlayer
        blend_space = internal_node.get_editor_property("blend_space")
        if not blend_space:
            continue

        name = blend_space.get_name()
        if not name.startswith(GLOBAL_BS_PREFIX):
            continue

        action = name[len(GLOBAL_BS_PREFIX) :]  # "BS_MM_Locomotion" → "Locomotion"
        bs_map[action] = blend_space

    unreal.log(f"[INFO] 템플릿 기반 AnimSequence 개수: {len(seq_map)}")
    unreal.log(f"[INFO] 템플릿 기반 BlendSpace 개수: {len(bs_map)}")
    return seq_map, bs_map


# ------------------------------------------------------------
# Pal AnimBP 에 Asset Override 적용
#   - AS_MM_Action → AS_<PalName>_Action
#   - BS_MM_Action → BS_<PalName>_Action
#   - 없으면 템플릿 자산 그대로 유지
# ------------------------------------------------------------
def apply_overrides_for_pal(
    pal_abp: unreal.AnimBlueprint,
    pal_folder: str,
    pal_name: str,
    template_abp : unreal.AnimBlueprint
) -> None:
    unreal.log("[START] pal ABP → 갱신")
    global_seq_map, global_bs_map = collect_template_assets_from_abp(template_abp)
    pal_anim_dir = f"{pal_folder}/Animation"

    # AnimSequence 오버라이드
    for action, global_anim in global_seq_map.items():
        pal_anim_name = f"{PAL_AS_PREFIX}{pal_name}_{action}"
        pal_anim_path = f"{pal_anim_dir}/{pal_anim_name}"
        if editor_lib.does_asset_exist(pal_anim_path):
            override_asset = editor_lib.load_asset(pal_anim_path)
        else:
            unreal.log_warning(
                f"[Seq OVERRIDE] {global_anim.get_name()} → 템플릿 유지 (Pal Anim 없음: {pal_anim_path})"
            )
            continue

        unreal.AnimationLibrary.add_node_asset_override(pal_abp, global_anim, override_asset, False)
        unreal.log(f"[Seq OVERRIDE] : {action} → {pal_anim_name}")

    # BlendSpace 오버라이드
    for action, global_bs in global_bs_map.items():
        pal_bs_name = f"{PAL_BS_PREFIX}{pal_name}_{action}"
        pal_bs_path = f"{pal_anim_dir}/{pal_bs_name}"
        if editor_lib.does_asset_exist(pal_bs_path):
            override_bs = editor_lib.load_asset(pal_bs_path)
        else:
            unreal.log_warning(
                f"[BS OVERRIDE] {global_bs.get_name()} → 템플릿 유지 (Pal BS 없음: {pal_bs_path})"
            )
            continue

        unreal.AnimationLibrary.add_node_asset_override(pal_abp, global_bs, override_bs, False)
        unreal.log(f"[Bs OVERRIDE] : {action} → {pal_bs_name}")

    # 컴파일 + 저장
    unreal.BlueprintEditorLibrary.compile_blueprint(pal_abp)
    unreal.log(f"[완료] Pal AnimBP 처리 완료: {pal_abp.get_path_name()}")
    #editor_lib.save_asset(pal_abp.get_path_name())


# ------------------------------------------------------------
# 메인 엔트리
#   - targetabp 로드 실패 시 즉시 return
#   - skeleton 없으면 return
#   - ABP 생성/로드 후 애니메이션 Override 적용
# ------------------------------------------------------------
def create_or_update_pal_anim_blueprint() -> None:
    unreal.log("==============================================")
    unreal.log("[START] 템플릿 ABP → Pal AnimBP 생성/갱신")

    # 1) 템플릿 AnimBlueprint 로드
    template_abp = find_asset(TARGET_ABP_PATH, unreal.AnimBlueprint)
    if not template_abp:
        unreal.log_error(f"[오류] 템플릿 AnimBlueprint 로드 실패: {TARGET_ABP_PATH}")
        return None
   
    pal_folder = f"{MONSTER_ROOT}/{PAL_NAME}"
    # 3) Skeleton 찾기
    mesh_dir = f"{pal_folder}/Mesh/SK_{PAL_NAME}_Skeleton"
    skel = find_asset(mesh_dir , unreal.Skeleton)
    if not skel:
        unreal.log_error(f"[오류] Skeleton 찾기 실패: {pal_folder}")
        return

    # 4) Pal 전용 AnimBlueprint 생성 또는 로드
    abp_path = f"{pal_folder}/ABP_{PAL_NAME}{TEMPLATE_ABP_NAME}"
    pal_abp = find_asset(abp_path , unreal.AnimBlueprint)
    if not pal_abp:
           parent_class = template_abp.generated_class()
           if not parent_class:
                  unreal.log_error(f"[오류] 템플릿 ABP generated_class 조회 실패: {TARGET_ABP_PATH}")
           
           factory = unreal.AnimBlueprintFactory()
           factory.set_editor_property("parent_class", parent_class)
           factory.set_editor_property("target_skeleton", skel)
           
           pal_abp = asset_tools.create_asset(
                  asset_name   = f"ABP_{PAL_NAME}{TEMPLATE_ABP_NAME}",
                  package_path = pal_folder,
                  asset_class  = unreal.AnimBlueprint,
                  factory      = factory,
           )

           if not pal_abp:
              unreal.log(f"[실패] Pal AnimBP 생성 실패 :   SK_{PAL_NAME}")
              return  
           
    # 5) AnimSequence / BlendSpace Override 설정
    apply_overrides_for_pal(
        pal_abp       = pal_abp,
        pal_folder    = pal_folder,
        pal_name      = PAL_NAME,
        template_abp = template_abp
    )

    unreal.log("[END] 템플릿 ABP → Pal AnimBP 생성/갱신 종료")
    unreal.log("==============================================")

if __name__ == "__main__":
   create_or_update_pal_anim_blueprint()
