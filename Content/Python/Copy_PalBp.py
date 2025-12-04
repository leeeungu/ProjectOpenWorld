import unreal
import PalConfig
from PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    GLOBAL_AS_PREFIX,
    GLOBAL_BS_PREFIX,
    PAL_AS_PREFIX,
    PAL_BS_PREFIX,
    find_asset,
)


# ============================================================
# 설정
#  - TARGET_BP_PATH          : 복사 기준이 될 템플릿 BP
#  - TARGET_ANIMBP_PATH      : Mesh에 세팅할 AnimBlueprint
#  - CHILD_BP_PREFIX         : Monster/<PAL_NAME>/ 아래에 생성할 BP 이름 prefix
#  - SkeletalMesh            : Monster/<PAL_NAME>/Mesh/SK_<PAL_NAME> 를 사용
# ============================================================

PAL_NAME = "Anubis"
TARGET_BP_PATH     = "/Game/Pal/Map/Test/Bp_TestCreature"
TARGET_ANIMBP_PATH = f"/Game/Pal/Model/Monster/{PAL_NAME}/ABP_Anubis"
CHILD_BP_PREFIX    = "Bp_" 

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_lib  = unreal.EditorAssetLibrary
registry    = unreal.AssetRegistryHelpers.get_asset_registry()

# ------------------------------------------------------------
# BP의 Mesh 컴포넌트에 SkeletalMesh + AnimBP 설정
#   - 기존 다른 프로퍼티는 건드리지 않는다
# ------------------------------------------------------------
def configure_bp_mesh_and_anim(
    blueprint: unreal.Blueprint,
    skeletal_mesh: unreal.SkeletalMesh | None,
    anim_bp: unreal.AnimBlueprint | None
) -> bool:

    # 1) AnimBlueprint의 GeneratedClass
    anim_class = anim_bp.generated_class()  # BlueprintGeneratedClass 

    # 2) Blueprint → GeneratedClass → Default Object(CDO)
    bp_class = blueprint.generated_class()         # BlueprintGeneratedClass
    cdo = unreal.get_default_object(bp_class)      # CDO (Actor) :contentReference[oaicite:2]{index=2}

    # 3) CDO에서 SkeletalMeshComponent 찾기
    comps = cdo.get_components_by_class(unreal.SkeletalMeshComponent)  
    # Array[ActorComponent] :contentReference[oaicite:3]{index=3}
    if not comps:
        return False

    comp: unreal.SkeletalMeshComponent = comps[0]

    # 4) SkeletalMesh / AnimBP 설정
    comp.set_skeletal_mesh_asset(skeletal_mesh)
    comp.set_editor_property("animation_mode", unreal.AnimationMode.ANIMATION_BLUEPRINT)
    comp.set_editor_property("anim_class", anim_class)

    # 5) Blueprint 컴파일 + 저장 (공식 API)
    unreal.BlueprintEditorLibrary.compile_blueprint(blueprint)       
    return True

# ------------------------------------------------------------
# 메인
# ------------------------------------------------------------
def create_or_update_blueprint():
    unreal.log("====================================================")
    unreal.log("[Start] Pal BP 생성/갱신 + Mesh/AnimBP 설정")

    # 템플릿 BP 로드
    template_bp = find_asset(TARGET_BP_PATH,unreal.Blueprint)
    if  not template_bp :
         return
    #unreal.log(f"[템플릿 BP 로드 성공] {template_bp.get_path_name()}")
    
    # 타겟 AnimBlueprint 로드
    anim_bp = find_asset(TARGET_ANIMBP_PATH, unreal.AnimBlueprint)
    if not anim_bp:
        return
    #unreal.log(f"[AnimBP 로드 성공] {anim_bp.get_path_name()}")
    
    # Pal SkeletalMesh 찾기: Monster/<PalName>/Mesh/SK_<PalName>
    pal_folder = f"{MONSTER_ROOT}/{PAL_NAME}"   # 예: /Game/Pal/Model/Monster/Anubis
    pal_name   = PAL_NAME

    mesh_package = f"{pal_folder}/Mesh/SK_{pal_name}"  # 패키지 이름
    skeletal_mesh = find_asset(mesh_package, unreal.SkeletalMesh)
    if not skeletal_mesh:
        return
    #unreal.log(f"[Mesh 로드 성공] {skeletal_mesh.get_path_name()}")

    # Pal용 BP 생성 또는 로드
    child_name        = f"{CHILD_BP_PREFIX}{pal_name}"  # 예: Bp_Anubis
    child_package     = f"{pal_folder}/{child_name}"    # 패키지 이름
    bp =  find_asset( child_package, unreal.Blueprint)
    if  not bp:
        bp = PalConfig.duplicate_asset(child_name,pal_folder ,template_bp)
        """
        asset_tools.duplicate_asset(
              asset_name   = child_name,  
              package_path = pal_folder,
             original_object = template_bp,
        )
        if  not bp:
            unreal.log_error(f"[오류] BP 복사 실패: {child_package}")
            return
        unreal.log(f"[BP 복사 생성] {bp.get_path_name()}")
        """
    #else:
        #unreal.log(f"[BP 로드 성공] {bp.get_path_name()}")
        
    configured = configure_bp_mesh_and_anim(bp, skeletal_mesh, anim_bp)

    unreal.log(f"[End] : BlueprintUdate 성공 {bp.get_path_name()}")
    unreal.log("====================================================")


if __name__ == "__main__":
    create_or_update_blueprint()
