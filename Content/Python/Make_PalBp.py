import unreal

# ============================================================
# 설정
# Animation 폴더가 있어야 pal로 취급됨
# Mesh도 자동 설정이니 조심하자
# ============================================================

MONSTER_ROOT        = "/Game/Pal/Model/Monster"

# ❗ 여기서는 "클래스 경로"를 사용합니다. (_C 붙은 경로)
PARENT_CLASS_PATH   = "/Game/Pal/Map/Test/Bp_TestCreature.Bp_TestCreature_C"

CHILD_BP_PREFIX     = "Bp_"   # Bp_팰이름


asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_lib  = unreal.EditorAssetLibrary
registry    = unreal.AssetRegistryHelpers.get_asset_registry()


# ------------------------------------------------------------
# 부모 클래스 로드
# ------------------------------------------------------------
def load_parent_class():
    # BlueprintGeneratedClass 로 직접 로드
    parent_class = unreal.load_object(None, PARENT_CLASS_PATH)
    if not parent_class:
        unreal.log_error(f"[오류] 부모 클래스 로드 실패: {PARENT_CLASS_PATH}")
        return None

    unreal.log(f"[ParentClass 로드 성공] {parent_class.get_name()}")
    return parent_class


# ------------------------------------------------------------
# Pal Mesh 찾기: Monster/<Pal>/Mesh/SK_<Pal>
# ------------------------------------------------------------
def find_skeletal_mesh(pal_folder, pal_name):
    mesh_path = f"{pal_folder}/Mesh/SK_{pal_name}"
    if editor_lib.does_asset_exist(mesh_path):
        mesh = editor_lib.load_asset(mesh_path)
        if isinstance(mesh, unreal.SkeletalMesh):
            return mesh
    unreal.log_warning(f"[Mesh 없음] {mesh_path}")
    return None


# ------------------------------------------------------------
# 자식 BP 생성 또는 기존 BP 로드
# ------------------------------------------------------------
def create_or_load_child_bp(pal_folder, pal_name, parent_class):
    child_name = f"{CHILD_BP_PREFIX}{pal_name}"
    child_path = f"{pal_folder}/{child_name}"

    # 이미 존재하면 로드해서 반환
    if editor_lib.does_asset_exist(child_path):
        unreal.log_warning(f"[SKIP 생성] 이미 존재: {child_path}")
        bp = editor_lib.load_asset(child_path)
        return bp if isinstance(bp, unreal.Blueprint) else None

    # BlueprintFactory + ParentClass 설정
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("ParentClass", parent_class)

    # create_asset(name, folder, asset_class, factory)
    new_bp = asset_tools.create_asset(
        asset_name   = child_name,
        package_path = pal_folder,
        asset_class  = unreal.Blueprint,
        factory      = factory,
    )

    if not new_bp:
        unreal.log_error(f"[오류] BP 생성 실패: {child_path}")
        return None

    unreal.log(f"[BP 생성됨] {child_path}")
    return new_bp


# ------------------------------------------------------------
# BP 의 Mesh 컴포넌트에 SkeletalMesh 설정
# (부모가 Character 계열이라고 전제)
# ------------------------------------------------------------
def set_bp_mesh(blueprint: unreal.Blueprint, skeletal_mesh: unreal.SkeletalMesh) -> bool:
    """
    주어진 Blueprint 에셋 안에서 SkeletalMeshComponent를 찾아
    그 컴포넌트의 skeletal_mesh 프로퍼티를 교체한다.
    - SubobjectDataSubsystem + SubobjectDataBlueprintFunctionLibrary 사용 (UE5 정석)
    """

    if not skeletal_mesh:
        return False

    # 서브오브젝트(컴포넌트) 정보 시스템
    subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    handles   = subsystem.k2_gather_subobject_data_for_blueprint(blueprint)

    if not handles:
        unreal.log_error(f"[오류] SubobjectDataHandle 없음: {blueprint.get_name()}")
        return False

    changed = False

    for handle in handles:
        # SubobjectData 와 실제 오브젝트 얻기
        data = unreal.SubobjectDataBlueprintFunctionLibrary.get_data(handle)
        obj  = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)

        if not obj:
            continue

        # 첫 번째는 보통 BP 자기 자신(Default__...Actor_C) 이라서 패스
        # 우리가 원하는 것은 SkeletalMeshComponent 만
        if not isinstance(obj, unreal.SkeletalMeshComponent):
            continue

        # 이미 같은 메쉬면 스킵
        current = obj.get_editor_property("skeletal_mesh")
        if current == skeletal_mesh:
            continue

        # SkeletalMesh 교체
        obj.set_editor_property("skeletal_mesh", skeletal_mesh)
        changed = True

        unreal.log(f"[Mesh 세팅] {blueprint.get_name()} :: {obj.get_name()} ← {skeletal_mesh.get_name()}")

        # 캐릭터 기준으로 메인 Mesh 하나만 있으면 되므로 첫 번째만 쓰고 종료
        break

    if changed:
        editor_lib.save_asset(blueprint.get_path_name())
    else:
        unreal.log_warning(f"[경고] {blueprint.get_name()} 안에서 SkeletalMeshComponent를 찾지 못했거나 이미 동일 Mesh")

    return changed


# ------------------------------------------------------------
# 메인
# ------------------------------------------------------------
def main():
    unreal.log("====================================================")
    unreal.log("[Start] Pal Child BP 생성 + Mesh 자동 세팅")
    unreal.log("====================================================")

    parent_class = load_parent_class()
    if not parent_class:
        return

    # Monster/<PalName> 하위 폴더 목록
    pal_folders = registry.get_sub_paths(MONSTER_ROOT, False)

    total_bp   = 0
    total_mesh = 0

    for pal_folder in pal_folders:
        # 예: /Game/Pal/Model/Monster/SheepBall
        pal_name    = pal_folder.split("/")[-1]
        anim_folder = f"{pal_folder}/Animation"

        # Animation 폴더가 있어야 "펠"로 취급
        if not editor_lib.does_directory_exist(anim_folder):
            continue

        # 1) 자식 BP 생성 또는 기존 BP 로드
        bp = create_or_load_child_bp(pal_folder, pal_name, parent_class)
        if not bp:
            continue
        total_bp += 1

        # 2) 해당 팔의 SkeletalMesh 찾기
        mesh = find_skeletal_mesh(pal_folder, pal_name)

        # 3) Mesh 설정
        if set_bp_mesh(bp, mesh):
            total_mesh += 1

    unreal.log("====================================================")
    unreal.log(f"처리된 BP 수     : {total_bp}")
    unreal.log(f"Mesh 세팅된 BP 수: {total_mesh}")
    unreal.log("====================================================")


if __name__ == "__main__":
    main()
