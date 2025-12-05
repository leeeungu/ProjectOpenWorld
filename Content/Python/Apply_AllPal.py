import unreal

import PalConfig
import Make_PalABP
import Child_PalBp
import Sync_PalAssets

from PalConfig import MONSTER_ROOT

editor_lib = unreal.EditorAssetLibrary
TEMPLATE_NAME = "_Monster"
Make_PalABP.TEMPLATE_ABP_NAME = TEMPLATE_NAME
Child_PalBp.CHILD_BP_POSTFIX = TEMPLATE_NAME
Child_PalBp.TARGET_BP_PATH     = f"/Game/Pal/Model/Global/Bp{TEMPLATE_NAME}Base"
Make_PalABP.TARGET_ABP_PATH   = f"{PalConfig.GLOBAL_DIR}/ABP_MM{TEMPLATE_NAME}Template"


def get_pal_names_from_monster_root() -> list[str]:
    """
    MONSTER_ROOT 하위의 1차 폴더명을 전부 Pal 이름으로 간주해서 반환.
    예: /Game/Pal/Model/Monster/Anubis, /Game/Pal/Model/Monster/Fox ...
    """
    root = MONSTER_ROOT  # "/Game/Pal/Model/Monster"

    # 모든 에셋/폴더 경로를 가져온 뒤, MONSTER_ROOT 기준 첫 세그먼트만 추출
    all_paths = editor_lib.list_assets(root, True, True)
    pal_names = set()

    for path in all_paths:
        # path 예: "/Game/Pal/Model/Monster/Anubis", "/Game/Pal/Model/Monster/Anubis/Animation/AS_..."
        rel = str(path)[len(root):].lstrip("/")
        if not rel:
            continue

        first = rel.split("/")[0]
        if not first:
            continue

        pal_folder = f"{root}/{first}"
        if editor_lib.does_directory_exist(pal_folder):
            pal_names.add(first)

    return sorted(pal_names)


def run_make_pal_abp_for_pal(pal_name: str) -> None:
    """
    특정 pal_name 에 대해 Make_PalABP 스크립트 실행.
    - Make_PalABP.PAL_NAME 을 교체한 뒤 create_or_update_pal_anim_blueprint 호출.
    """
    unreal.log(f"---------- [Make_PalABP] 시작: {pal_name} ----------")

    # 대상 Pal 이름 주입
    Make_PalABP.PAL_NAME = pal_name

    # Make_PalABP 내부에서 PAL_NAME 을 사용하므로, 여기서 바로 메인 함수 호출
    Make_PalABP.create_or_update_pal_anim_blueprint()

    unreal.log(f"---------- [Make_PalABP] 종료: {pal_name} ----------")


def run_child_pal_bp_for_pal(pal_name: str) -> None:
    """
    특정 pal_name 에 대해 Child_PalBp 스크립트 실행.
    - Child_PalBp.PAL_NAME 및 TARGET_ANIMBP_PATH 를 pal_name 기준으로 재설정 후
      create_or_update_blueprint 호출.
    """
    unreal.log(f"---------- [Child_PalBp] 시작: {pal_name} ----------")

    Child_PalBp.PAL_NAME = pal_name

    # Child_PalBp 내 상수/포스트픽스 활용해서 TARGET_ANIMBP_PATH 재계산
    pal_folder = f"{MONSTER_ROOT}/{pal_name}"
    Child_PalBp.TARGET_ANIMBP_PATH = (
        f"{pal_folder}/ABP_{pal_name}{Child_PalBp.CHILD_BP_POSTFIX}"
    )

    Child_PalBp.create_or_update_blueprint()

    unreal.log(f"---------- [Child_PalBp] 종료: {pal_name} ----------")


def main() -> None:
    unreal.log("==============================================")
    unreal.log("[START] MONSTER_ROOT 전체 팔에 대해 ABP → Child BP 일괄 생성/갱신")
    unreal.log(f"[INFO] MONSTER_ROOT = {MONSTER_ROOT}")

    pal_names = get_pal_names_from_monster_root()
    unreal.log(f"[INFO] 발견된 Pal 수: {len(pal_names)}")
    unreal.log(f"[INFO] Pal 목록: {', '.join(pal_names)}")

    for pal_name in pal_names:
        unreal.log("----------------------------------------------")
        unreal.log(f"[Pal 처리] {pal_name}")

        pal_folder = f"{MONSTER_ROOT}/{pal_name}"
        if not editor_lib.does_directory_exist(pal_folder):
            unreal.log_warning(f"[SKIP] 폴더 없음: {pal_folder}")
            continue

        # 1) AnimBlueprint 생성/갱신
        run_make_pal_abp_for_pal(pal_name)

        # 2) Child BP 생성/갱신
        run_child_pal_bp_for_pal(pal_name)
        Sync_PalAssets.apply_bp_capsule_and_mesh_to_siblings(pal_name)

    unreal.log("[END] 전체 팔 처리 완료")
    unreal.log("==============================================")


if __name__ == "__main__":
    main()
