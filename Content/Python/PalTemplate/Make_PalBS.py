import unreal
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    GLOBAL_AS_PREFIX,
    GLOBAL_BS_PREFIX,
    PAL_AS_PREFIX,
    PAL_BS_PREFIX,
    find_asset,
    CONFIG_PAL_NAME,
    BS_WEIGHT_SPEED
)

from typing import Type, TypeVar

# ------------------------------------------------------------------
# 사용자 설정
# ------------------------------------------------------------------

# 작업 대상 팔 / 이름  필요한 팔 이름으로 수정
PAL_NAME: str = CONFIG_PAL_NAME 

# 복사 대상 Global BlendSpace 경로 (ObjectPath 전체)
TARGET_ASSET_PATH: str = f"{GLOBAL_ANIM_DIR}/BS_MM_Locomotion"


# ------------------------------------------------------------------
# 공용 핸들
# ------------------------------------------------------------------

editor_lib  = unreal.EditorAssetLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
registry    = unreal.AssetRegistryHelpers.get_asset_registry()


# ------------------------------------------------------------------
# BS 축 설정 (기존 configure_axes_for_pal_bs 로직 그대로)
# ------------------------------------------------------------------

def configure_axes_for_pal_bs(pal_bs: unreal.BlendSpace) -> None:
    """
    축 설정:
      - Vertical(Y): Min=-1, Max=1, GridNum=2, SnapToGrid=True (이름 기본값 유지)
      - Horizontal(X): Name="Speed", Min=0, Max=1, GridNum=4, SnapToGrid=True
    """
    params = list(pal_bs.get_editor_property("blend_parameters"))  # Array[BlendParameter]

    if len(params) < 2:
        unreal.log_warning(f"[WARN] BlendParameters 개수가 2 미만: {pal_bs.get_path_name()}")
        return

    horiz = params[0]  # X 축
    vert  = params[1]  # Y 축

    # Vertical (Y)
    vert.set_editor_property("min", -1.0)
    vert.set_editor_property("max",  1.0)
    vert.set_editor_property("grid_num", 2)
    vert.set_editor_property("snap_to_grid", True)

    # Horizontal (X)
    horiz.set_editor_property("display_name", "Speed")
    horiz.set_editor_property("min",  0.0)
    horiz.set_editor_property("max",  1.0)
    horiz.set_editor_property("grid_num", 4)
    horiz.set_editor_property("snap_to_grid", True)

    pal_bs.set_editor_property("blend_parameters", params)
    pal_bs.set_editor_property("target_weight_interpolation_speed_per_sec", BS_WEIGHT_SPEED)

# ------------------------------------------------------------------
# Global BS → Pal BS sample_data 복사 (애니 이름 치환)
# ------------------------------------------------------------------

def build_pal_samples_from_global(
    global_bs: unreal.BlendSpace,
    pal_bs: unreal.BlendSpace,
    pal_name: str,
    pal_anim_dir: str
) -> None:
    """
    Global BS의 sample_data를 그대로 복사하되,
    Animation만 AS_MM_* → AS_<PalName>_* 로 교체해서 셋업.
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
            unreal.log_warning(
                f"[WARN] 예상 외 애니 이름, 그대로 사용 시도: {anim_name} (Pal={pal_name})"
            )
            pal_anim_name = anim_name

        pal_anim_path = f"{pal_anim_dir}/{pal_anim_name}"
        pal_anim = editor_lib.load_asset(pal_anim_path)

        if not pal_anim:
            unreal.log_warning(
                f"[SKIP] Pal Anim 없음: {pal_anim_path} (GlobalAnim={anim_name})"
            )
            continue

        # 새 BlendSample 생성 및 복사
        new_sample = unreal.BlendSample()
        new_sample.set_editor_property("animation", pal_anim)
        new_sample.set_editor_property(
            "sample_value",
            sample.get_editor_property("sample_value")
        )
        new_sample.set_editor_property(
            "rate_scale",
            sample.get_editor_property("rate_scale")
        )
        new_sample.set_editor_property(
            "include_in_analyse_all",
            sample.get_editor_property("include_in_analyse_all")
        )

        new_samples.append(new_sample)

    pal_bs.set_editor_property("sample_data", new_samples)
    unreal.log(f"[INFO] AnimationData 적용: {len(new_samples)}개")


# ------------------------------------------------------------------
# Global BS 1개(TARGET_ASSET_PATH) → Pal BS 생성/갱신
# ------------------------------------------------------------------

def create_or_update_pal_bs_from_target(global_bs_path: str, pal_name: str) -> None:
    # 1) Global BlendSpace 로드
    global_bs = find_asset(global_bs_path, unreal.BlendSpace)
    if not global_bs:
        unreal.log_error(f"[오류] Global BlendSpace 로드 실패: {global_bs_path}")
        return

    # 2) Pal 경로 계산
    pal_folder   = f"{MONSTER_ROOT}/{pal_name}"
    pal_anim_dir = f"{pal_folder}/Animation"

    # 3) Skeleton 찾기
    skel = find_asset(pal_folder + "/Mesh/SK_" +PAL_NAME+"_Skeleton" , unreal.Skeleton)
    if not skel:
        unreal.log_error(f"[오류] Skeleton 찾기 실패: {pal_folder}")
        return

    # 4) Global BS 이름에서 Action 추출
    global_name = global_bs.get_name()  # 예: "BS_MM_Locomotion"
    if global_name.startswith(GLOBAL_BS_PREFIX):
        action = global_name[len(GLOBAL_BS_PREFIX):]  # "Locomotion"
    else:
        unreal.log_warning(f"[WARN] 예상치 못한 Global BS 이름 패턴: {global_name}")
        action = global_name

    pal_bs_name = f"{PAL_BS_PREFIX}{pal_name}_{action}"
    pal_bs_path = f"{pal_anim_dir}/{pal_bs_name}"

    # 5) Pal BS 생성 또는 로드
    if editor_lib.does_asset_exist(pal_bs_path):
        pal_bs = editor_lib.load_asset(pal_bs_path)
        unreal.log(f"[INFO] 기존 Pal BS 로드: {pal_bs_path}")
    else:
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

        unreal.log(f"[생성] {pal_bs_path}")

    # 6) 축 설정 복사
    configure_axes_for_pal_bs(pal_bs)
    
    # 7) SampleData 복사 (애니 이름 치환)
    build_pal_samples_from_global(global_bs, pal_bs, pal_name, pal_anim_dir)

    # 8) 저장/컴파일은 하지 않음 (요청 사항)
    #    → 에디터 상에서 해당 BS 패키지가 Dirty 상태로 남음.
    unreal.log(f"[DONE] Pal BS 구성 완료 (저장): {pal_bs_path}")
    unreal.EditorAssetLibrary.save_asset(pal_bs_path)


# ------------------------------------------------------------------
# main
# ------------------------------------------------------------------

def main():
    unreal.log(f"[Python] Start : 펠 네임 {CONFIG_PAL_NAME} Blenda Space 생성")
    unreal.log("==============================================")
    unreal.log("[START] Global BS → Pal BS 생성 스크립트 (단일 TARGET_ASSET_PATH)")

    create_or_update_pal_bs_from_target(TARGET_ASSET_PATH, PAL_NAME)

    unreal.log("[완료] 처리 종료 (저장은 수행하지 않음)")
    unreal.log("==============================================")
    unreal.log(f"[Python] Start : 펠 네임 {CONFIG_PAL_NAME} Blenda Space 생성 완료")


if __name__ == "__main__":
    main()
