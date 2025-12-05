import unreal
from PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    GLOBAL_AS_PREFIX,
    GLOBAL_BS_PREFIX,
    PAL_AS_PREFIX,
    PAL_BS_PREFIX,
    find_asset
)

# ============================================================
# 설정
#  - PAL_NAME        : 작업할 팔 이름
#  - PAL_ABP_PREFIX  : 팔 AnimBlueprint 이름 접두어
#  - PAL_BP_PREFIX   : 팔 Blueprint 이름 접두어
#    예) MONSTER_ROOT/<PAL_NAME>/ABP_<PAL_NAME>
#        MONSTER_ROOT/<PAL_NAME>/Bp_<PAL_NAME>
# ============================================================

PAL_NAME       = "Carbunclo"   # 필요 시 외부에서 수정
PAL_ABP_PREFIX = "ABP_"
PAL_BP_PREFIX  = "Bp_"

editor_lib = unreal.EditorAssetLibrary
registry   = unreal.AssetRegistryHelpers.get_asset_registry()

# ------------------------------------------------------------
# 2. 기준 Bp_팔이름 의 Capsule / Mesh 높이를
#    같은 폴더의 다른 Blueprint 에 그대로 적용
# ------------------------------------------------------------
def apply_bp_capsule_and_mesh_to_siblings(pal_name: str) -> None:
    pal_folder = f"{MONSTER_ROOT}/{pal_name}"
    base_bp_name = f"{PAL_BP_PREFIX}{pal_name}"
    base_bp_path = f"{pal_folder}/{base_bp_name}"

    if not editor_lib.does_asset_exist(base_bp_path):
        unreal.log_warning(f"[BP Sync] 기준 BP 없음: {base_bp_path}")
        return

    base_bp = editor_lib.load_asset(base_bp_path)
    if not isinstance(base_bp, unreal.Blueprint):
        unreal.log_warning(f"[BP Sync] Blueprint 가 아님: {base_bp_path}")
        return

    bp_class = base_bp.generated_class()
    if not bp_class:
        unreal.log_warning(f"[BP Sync] generated_class 없음: {base_bp_path}")
        return

    base_cdo = unreal.get_default_object(bp_class)

    # ---------- 1) 기준 CDO 에서 Capsule / Mesh 정보 추출 ----------
    base_capsules = base_cdo.get_components_by_class(unreal.CapsuleComponent)
    if not base_capsules:
        unreal.log_warning(f"[BP Sync] 기준 BP 에 CapsuleComponent 없음: {base_bp_path}")
        return

    base_capsule: unreal.CapsuleComponent = base_capsules[0]
    base_radius = base_capsule.get_unscaled_capsule_radius()
    base_half_height = base_capsule.get_unscaled_capsule_half_height()

    base_meshes = base_cdo.get_components_by_class(unreal.SkeletalMeshComponent)
    if not base_meshes:
        unreal.log_warning(f"[BP Sync] 기준 BP 에 SkeletalMeshComponent 없음: {base_bp_path}")
        return

    base_mesh: unreal.SkeletalMeshComponent = base_meshes[0]
    base_loc = base_mesh.get_editor_property("relative_location")

    unreal.log(
        f"[BP Sync] 기준 BP: {base_bp.get_path_name()} | "
        f"Capsule(R={base_radius}, H={base_half_height}) | "
        f"Mesh Z={base_loc.z}"
    )

    # ---------- 2) 같은 폴더의 다른 Blueprint 들에 복사 ----------
    asset_paths = editor_lib.list_assets(
        pal_folder,
        recursive=False,
        include_folder=False
    )

    for asset_path in asset_paths:
        bp = editor_lib.load_asset(asset_path)
        if not bp:
            continue
        #if not isinstance(bp, unreal.Blueprint):
        #    continue
        if isinstance(bp, unreal.AnimBlueprint):
            continue

        # 기준 BP 는 스킵
        if bp.get_name() == base_bp_name:
            continue

        bp_class = bp.generated_class()
        if not bp_class:
            continue

        cdo = unreal.get_default_object(bp_class)

        capsules = cdo.get_components_by_class(unreal.CapsuleComponent)
        meshes   = cdo.get_components_by_class(unreal.SkeletalMeshComponent)

        if not capsules or not meshes:
            unreal.log_warning(
                f"[BP Sync] Capsule/Mesh 없음, 스킵: {bp.get_path_name()}"
            )
            continue

        capsule: unreal.CapsuleComponent = capsules[0]
        mesh: unreal.SkeletalMeshComponent = meshes[0]

        # Capsule 높이 / 반지름 동기화
        capsule.set_capsule_radius(base_radius, True)
        capsule.set_capsule_half_height(base_half_height, True)

        # Mesh 상대 위치 Z만 동기화 (높이)
        loc = mesh.get_editor_property("relative_location")
        loc.z = base_loc.z
        mesh.set_editor_property("relative_location", loc)

        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        #editor_lib.save_loaded_asset(bp)

        unreal.log(
            f"[BP Sync] 적용 완료 → {bp.get_path_name()} "
            f"(R={base_radius}, H={base_half_height}, MeshZ={loc.z})"
        )

    unreal.log("[BP Sync] 완료")


# ------------------------------------------------------------
# 메인 (편의용): 현재 PAL_NAME 에 대해 두 동기화 모두 수행
# ------------------------------------------------------------
def main() -> None:
    unreal.log("==============================================")
    unreal.log(f"[START] Pal 동기화: {PAL_NAME}")
    unreal.log("==============================================")

    apply_bp_capsule_and_mesh_to_siblings(PAL_NAME)

    unreal.log("==============================================")
    unreal.log(f"[END] Pal 동기화 완료: {PAL_NAME}")
    unreal.log("==============================================")


if __name__ == "__main__":
    main()
