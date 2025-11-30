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
# ============================================================
# 설정
# ============================================================

#GLOBAL_ANIM_DIR   = "/Game/Pal/Model/Global/Animation"
#MONSTER_ROOT      = "/Game/Pal/Model/Monster"

# Global 이름 규칙
#GLOBAL_AS_PREFIX  = "AS_MM_"   # Global AnimSequence
#GLOBAL_BS_PREFIX  = "BS_MM_"   # Global BlendSpace

# Pal 이름 규칙
#PAL_AS_PREFIX     = "AS_"      # AS_<PalName>_<Action>
#PAL_BS_PREFIX     = "BS_"      # BS_<PalName>_<Action>

editor_lib  = unreal.EditorAssetLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
registry    = unreal.AssetRegistryHelpers.get_asset_registry()


# ============================================================
# 공통 유틸
# ============================================================

def find_pal_skeleton(pal_folder: str):
    """
    각 팰 폴더의 Mesh 하위에서 SkeletalMesh 하나 찾아 Skeleton 반환.
    예: /Game/Pal/Model/Monster/Kitsunebi/Mesh
    """
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


def collect_global_blendspaces():
    """
    Global_Animation 폴더에서 BS_MM_* BlendSpace 로드.
    -> action 기준 dict 로 반환.
    """
    asset_paths = editor_lib.list_assets(GLOBAL_ANIM_DIR, recursive=False)
    result = []

    for path in asset_paths:
        name = path.split("/")[-1]
        asset = editor_lib.load_asset(path)
        if not asset:
            continue

        if not isinstance(asset, (unreal.BlendSpace, unreal.BlendSpace1D)):
            continue

        if not name.startswith(GLOBAL_BS_PREFIX):
            continue

        result.append(asset)

    unreal.log(f"[INFO] Global BlendSpace 개수: {len(result)}")
    return result


def configure_axes_for_pal_bs(pal_bs: unreal.BlendSpace):
    """
    축 설정:
      - Vertical(Y):  Min=-1, Max=1, GridNum=2, SnapToGrid=True (이름은 기본값 유지)
      - Horizontal(X): Name="Speed", Min=-1, Max=1, GridNum=4, SnapToGrid=True
    """
    params = list(pal_bs.get_editor_property("blend_parameters"))  # Array[BlendParameter]

    if len(params) < 2:
        unreal.log_warning(f"[WARN] BlendParameters 개수가 2 미만: {pal_bs.get_path_name()}")
        return

    horiz = params[0]  # X축
    vert  = params[1]  # Y축

    # Vertical (struct 필드 직접 수정)
    vert.set_editor_property("min", -1.0)
    vert.set_editor_property("max",  1.0)
    vert.set_editor_property("grid_num",  2)
    vert.set_editor_property("snap_to_grid", True)
    # vert.display_name 은 기본값(None) 유지

    # Horizontal
    horiz.set_editor_property("display_name", "Speed")
    horiz.set_editor_property("min",0)
    horiz.set_editor_property("max",  1.0)
    horiz.set_editor_property("grid_num",  4)


    # 수정된 배열 다시 반영
    pal_bs.set_editor_property("blend_parameters", params)
   

def build_pal_samples_from_global(
    global_bs: unreal.BlendSpace,
    pal_bs: unreal.BlendSpace,
    pal_name: str,
    pal_anim_dir: str
):
    """
    Global BS의 sample_data 구성 그대로 복사하되,
    Animation만 AS_MM_* → AS_<PalName>_* 로 교체.

    - Global: AS_MM_Walk
    - Pal:    AS_<PalName>_Walk
    """

    global_samples = list(global_bs.get_editor_property("sample_data"))  # Array[BlendSample]
    new_samples = []

    for sample in global_samples:
        anim = sample.get_editor_property("animation")
        if not anim:
            continue

        anim_name = anim.get_name()  # 예: "AS_MM_Walk"

        # MM prefix를 Pal prefix로 변경
        if anim_name.startswith(GLOBAL_AS_PREFIX):
            action = anim_name[len(GLOBAL_AS_PREFIX):]  # "Walk"
            pal_anim_name = f"{PAL_AS_PREFIX}{pal_name}_{action}"
        else:
            # 규칙에서 벗어나면 이름 그대로 사용 시도
            unreal.log_warning(
                f"[WARN] 예상 외 애니 이름, 그대로 사용 시도: {anim_name} (Pal={pal_name})"
            )
            pal_anim_name = anim_name

        pal_anim_path = f"{pal_anim_dir}/{pal_anim_name}.{pal_anim_name}"
        pal_anim = editor_lib.load_asset(pal_anim_path)

        if not pal_anim:
            unreal.log_warning(
                f"[SKIP] Pal Anim 없음: {pal_anim_path} (GlobalAnim={anim_name})"
            )
            continue

        # 새 BlendSample 생성
        new_sample = unreal.BlendSample()
        new_sample.set_editor_property("animation",  pal_anim)
        new_sample.set_editor_property("sample_value", sample.get_editor_property("sample_value"))
        new_sample.set_editor_property("rate_scale", sample.get_editor_property("rate_scale"))
        new_sample.set_editor_property("include_in_analyse_all", sample.get_editor_property("include_in_analyse_all"))

        new_samples.append(new_sample)

    pal_bs.set_editor_property("sample_data", new_samples)
    unreal.log(f"  [INFO] SampleData 적용: {len(new_samples)}개")


# ============================================================
# Pal용 BlendSpace 생성 / 업데이트
# ============================================================

def create_or_update_pal_bs(
    global_bs: unreal.BlendSpace,
    pal_folder: str,
    pal_name: str
):
    pal_anim_dir = f"{pal_folder}/Animation"
    if not editor_lib.does_directory_exist(pal_anim_dir):
        unreal.log_warning(f"[SKIP] Animation 폴더 없음: {pal_anim_dir}")
        return

    skel = find_pal_skeleton(pal_folder)
    if not skel:
        return

    # Global BS 이름에서 Action 추출
    global_name = global_bs.get_name()  # 예: "BS_MM_Walk"
    if global_name.startswith(GLOBAL_BS_PREFIX):
        action = global_name[len(GLOBAL_BS_PREFIX):]
    else:
        unreal.log_warning(f"[WARN] 예상치 못한 Global BS 이름 패턴: {global_name}")
        action = global_name

    pal_bs_name = f"{PAL_BS_PREFIX}{pal_name}_{action}"
    pal_bs_path = f"{pal_anim_dir}/{pal_bs_name}.{pal_bs_name}"

    # 이미 존재하면 로드 후 재설정
    if editor_lib.does_asset_exist(pal_bs_path):
        unreal.log_warning(f"[SKIP] 이미 BS 존재, 재설정만 수행: {pal_bs_path}")
        pal_bs = editor_lib.load_asset(pal_bs_path)
    else:
        # 새 BlendSpace 생성
        factory = unreal.BlendSpaceFactoryNew()
        factory.set_editor_property("target_skeleton", skel)

        pal_bs = asset_tools.create_asset(
            asset_name   = pal_bs_name,
            package_path = pal_anim_dir,
            asset_class  = unreal.BlendSpace,
            factory      = factory
        )

        if not pal_bs:
            unreal.log_error(f"[오류] Pal BS 생성 실패: {pal_bs_name}")
            return

        unreal.log(f"  [생성] {pal_bs_path}")

    # 축 설정
    configure_axes_for_pal_bs(pal_bs)

    # SampleData 구성 복사
    build_pal_samples_from_global(global_bs, pal_bs, pal_name, pal_anim_dir)

    # 저장
    #unreal.BlueprintEditorLibrary.compile_blueprint(pal_bs)
    editor_lib.save_asset(pal_bs_path)


# ============================================================
# main
# ============================================================

def main():
    unreal.log("==============================================")
    unreal.log("[START] Global BS → Pal BS 생성 스크립트")
    unreal.log("==============================================")

    global_bs_list = collect_global_blendspaces()
    if not global_bs_list:
        unreal.log_error("[오류] Global BlendSpace 없음")
        return

    #pal_folders = registry.get_sub_paths(MONSTER_ROOT, recurse=False)
    pal_folder =MONSTER_ROOT + "/" + PAL_NAME

    #for pal_folder in pal_folders:
    pal_name = pal_folder.split("/")[-1]
    unreal.log(f"------ {pal_name} 처리 시작 ------")

    for global_bs in global_bs_list:
       create_or_update_pal_bs(global_bs, pal_folder, pal_name)

    unreal.log(f"------ {pal_name} 처리 종료 ------")

    unreal.log("==============================================")
    unreal.log("[완료] Global BS → Pal BS 전체 처리 종료")
    unreal.log("==============================================")


if __name__ == "__main__":
    main()
