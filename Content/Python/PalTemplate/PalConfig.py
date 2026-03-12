import unreal
import json

# 모든 스크립트에서 공유할 기본 경로
GLOBAL_ANIM_DIR = "/Game/Pal/Model/Global/Animation"
GLOBAL_DIR = "/Game/Pal/Model/Global"
MONSTER_ROOT = "/Game/Pal/Model/Monster"

# 기본 타겟 팔 이름 (필요 시 스크립트에서 변경 가능)


GLOBAL_AS_PREFIX  = "AS_MM_"
GLOBAL_BS_PREFIX  = "BS_MM_"
PAL_AS_PREFIX     = "AS_"
PAL_BS_PREFIX     = "BS_"
PAL_BP_PREFIX     = "Bp_"   # Bp_팰이름
CONFIG_PAL_NAME   = "HawkBird"
PAL_ATTACK_DT_STRUCT_PATH = "/Script/ProjectOpenWorld.PalAttackDataTable"
ANIM_SEQUENCE_NAMES = ["FarSkill_Start", "FarSkill_StartLoop",
                       "FarSkill_Action", "FarSkill_ActionLoop", "FarSkill_End"]  # 접두사 제외한 꼬리만
BS_WEIGHT_SPEED = 5

from typing import Type, TypeVar

T = TypeVar("T", bound=unreal.Object)

# asset_path의 classtype 에 해당되는 Asset 찾는 함수
def find_asset(asset_path: str, classtype: Type[T]) -> T | None:
    asset = unreal.EditorAssetLibrary.load_asset(asset_path = asset_path)
    # 1. 에셋 자체가 없음
    if asset is None:
        unreal.log_warning(f"[Load Failed] 에셋을 찾을 수 없음: {asset_path}")
        return None

    # 2. 에셋은 있으나 타입이 다름
    if not isinstance(asset, classtype):
        unreal.log_warning(
            f"[Type Mismatch] 로드됨: {asset.get_class()} / 기대 타입: {classtype.__name__} / 경로: {asset_path}"
        )
        return None
    unreal.log(f"[Load 성공] : {asset.get_path_name()}")
    return asset

def duplicate_asset(  child_name   : str,  pal_folder : str,  template_bp: unreal.Object) -> unreal.Object | None:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = asset_tools.duplicate_asset(
              asset_name   = child_name,  
              package_path = pal_folder,
             original_object = template_bp,
    )
    if asset is None:
        unreal.log_error(f"[오류] BP 복사 실패: {child_name}")
        return None
    unreal.log(f"[성공] BP 복사 성공: {asset.get_path_name()}")
    return asset

def child_asset(  child_name   : str,  pal_folder : str,  template_bp: unreal.Object) -> unreal.Object | None:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    # 1) 부모 클래스: 템플릿 BP의 GeneratedClass
    parent_class = template_bp.generated_class()
    if not parent_class:
        unreal.log_error(
            f"[오류] 템플릿 BP generated_class 조회 실패: {template_bp.get_path_name()}"
        )
        return None

    # 2) BlueprintFactory 생성 후 parent_class 지정
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)

    # 3) 자식 Blueprint 생성
    child_bp = asset_tools.create_asset(
        asset_name   = child_name,
        package_path = pal_folder,
        asset_class  = unreal.Blueprint,
        factory      = factory,
    )

    if not child_bp:
        unreal.log_error(f"[오류] 자식 BP 생성 실패: {child_name}")
        return None

    unreal.log(f"[성공] 자식 BP 생성: {child_bp.get_path_name()}")
    return child_bp

def get_pal_names_from_monster_root() -> list[str]:
    """
    MONSTER_ROOT 하위의 1차 폴더명을 전부 Pal 이름으로 간주해서 반환.
    예: /Game/Pal/Model/Monster/Anubis, /Game/Pal/Model/Monster/Fox ...
    """
    editor_lib = unreal.EditorAssetLibrary
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

def load_or_create_datatable(dt_folder: str, dt_name: str, structStr : str) -> unreal.DataTable:
    
    if not unreal.EditorAssetLibrary.does_directory_exist(dt_folder):
        unreal.EditorAssetLibrary.make_directory(dt_folder)    
    dt_asset_path = f"{dt_folder}/{dt_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(dt_asset_path):
        dt = unreal.EditorAssetLibrary.load_asset(dt_asset_path)
        if not isinstance(dt, unreal.DataTable):
            raise RuntimeError(f"경로에 DataTable이 아닌 에셋이 존재합니다: {dt_asset_path}")
        return dt
    candidates = [
        f"/Script/ProjectOpenWorld.{structStr}",
        structStr,  # find_object fallback 용
    ]
    row_struct = None
    for c in candidates:
        try:
            obj = unreal.load_object(None, c)
            if obj:
                row_struct =  obj
        except Exception:
            pass

        try:
            obj = unreal.find_object(None, c)
            if obj:
                row_struct = obj
        except Exception:
            pass

    if not row_struct:
        raise RuntimeError(
            f"RowStruct '{structStr}'을 찾지 못했습니다. "
            f"구조체가 컴파일 되었는 지 확인해세요."
        )

    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", row_struct)
    unreal.log("make Struct")
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    dt = asset_tools.create_asset(dt_name, dt_folder, unreal.DataTable, factory)
    if not dt:
        raise RuntimeError(f"DataTable 생성 실패: {dt_asset_path}")
    unreal.EditorAssetLibrary.save_loaded_asset(dt)
    return dt


def Get_DtRows_as_list(dt: unreal.DataTable) -> list[dict]:
    s = dt.export_to_json_string()
    if not s:
        return []
    data = json.loads(s)

    # UE는 보통 list 포맷으로 내보냅니다. 혹시 dict면 list로 normalize.
    if isinstance(data, list):
        return data
    if isinstance(data, dict):
        rows = []
        for k, v in data.items():
            if isinstance(v, dict):
                r = dict(v)
                r.setdefault("Name", k)
                rows.append(r)
        return rows

    return []

def Apply_dt_from_rows_list(dt: unreal.DataTable, rows: list[dict]) -> bool:
   
    row_struct = dt.get_row_struct()
    json_str = json.dumps(rows, ensure_ascii=False, indent=2)

    ok = dt.fill_from_json_string(json_str, row_struct)
   
    return bool(ok)

def Get_MonsterBp(MonsterName : str) -> unreal.Object:
    return unreal.load_asset(f"{MONSTER_ROOT}/{MonsterName}/{PAL_BP_PREFIX}{MonsterName}_Monster")  
    

def Get_MonsterLevelDT(MonsterName : str) -> unreal.Object:
    return unreal.load_asset(f"{MONSTER_ROOT}/{MonsterName}/DataTable/DT_{MonsterName}_LevelData")  
    return None

def Get_PalBp(PalName : str) -> unreal.Object:
    return unreal.load_asset(f"{MONSTER_ROOT}/{PalName}/{PAL_BP_PREFIX}{PalName}_Pal")  
    
    
"""
def main():
   pal_name  = "Anubis"
   asset_path = MONSTER_ROOT +"/" + pal_name + "/Mesh" + "/SK_"+ pal_name + "_Skeleton"
   Mesh = find_asset( asset_path, unreal.Skeleton)
   if Mesh:
       unreal.log(unreal.EditorAssetLibrary.get_path_name_for_loaded_asset(Mesh))

if __name__ == "__main__":
    main()
"""
