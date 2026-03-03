import unreal
import json
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    MONSTER_ROOT,
    PAL_BP_PREFIX,
    CONFIG_PAL_NAME,
    PAL_ATTACK_DT_STRUCT_PATH,
    ANIM_SEQUENCE_NAMES,
)

PAL_NAME = CONFIG_PAL_NAME
# -----------------------------
# 경로/에셋 유틸
# -----------------------------
def ensure_directory(dir_path: str):
    if not unreal.EditorAssetLibrary.does_directory_exist(dir_path):
        unreal.EditorAssetLibrary.make_directory(dir_path)

def list_assets_of_class(folder: str, cls, recursive: bool = True):
    if not unreal.EditorAssetLibrary.does_directory_exist(folder):
        return []

    prefix = f"AS_{PAL_NAME}_"

    # 1) 원하는 이름 목록을 '순서 유지'한 채로 full name으로 정규화
    wanted_names_in_order = []
    for n in ANIM_SEQUENCE_NAMES:
        
        if not isinstance(n, str) or not n:
            continue
        full = n if n.startswith(prefix) else f"{prefix}{n}"
        wanted_names_in_order.append(full)
        
    
    asset_paths = unreal.EditorAssetLibrary.list_assets(folder, recursive=recursive, include_folder=False)


    out = []
    missing = []
    
    for asset_name in wanted_names_in_order:
        p = asset_name
        if not p:
            missing.append(asset_name)
            continue

        obj = unreal.EditorAssetLibrary.load_asset(f"{folder}/{p}")
        if isinstance(obj, cls):
            out.append(obj)
        else:
            unreal.log_warning(f"[Anim] 타입 불일치: {asset_name} (기대={cls.__name__})")

    if missing:
        unreal.log_warning(f"[Anim] 폴더에서 못 찾은 AnimSequence: {missing}")
    return out

def get_object_path(asset_obj) -> str:
    # "/Game/.../Asset.Asset" 형태
    return unreal.EditorAssetLibrary.get_path_name_for_loaded_asset(asset_obj)

def does_asset_exist(asset_path_no_dot: str) -> bool:
    # "/Game/.../AssetName" 경로 기준으로 체크
    return unreal.EditorAssetLibrary.does_asset_exist(asset_path_no_dot)

def load_asset(asset_path_no_dot: str):
    return unreal.EditorAssetLibrary.load_asset(asset_path_no_dot)

def save_asset(asset_obj):
    unreal.EditorAssetLibrary.save_loaded_asset(asset_obj)

def resolve_row_struct_fpal_attackdatatable():
    """
    FPalAttackDataTable의 리플렉션 이름은 보통 'PalAttackDataTable'(F 제거)입니다.
    모듈명은 프로젝트명(=SystemLibrary.get_project_name())으로 우선 추정합니다.
    """
    struct_name = "PalMonsterLevelData"  # FPalMonsterLevelData -> PalMonsterLevelData
    ##project_module = unreal.SystemLibrary.get_project_name()

    # 1) 가장 흔한 형태: /Script/<ProjectModule>.<StructName>
    candidates = [
        PAL_ATTACK_DT_STRUCT_PATH,
        struct_name,  # find_object fallback 용
    ]

    for c in candidates:
        try:
            obj = unreal.load_object(None, c)
            if obj:
                return obj
        except Exception:
            pass

        try:
            obj = unreal.find_object(None, c)
            if obj:
                return obj
        except Exception:
            pass

    raise RuntimeError(
        f"RowStruct 'FPalMonsterLevelData'을 찾지 못했습니다. "
        f"resolve_row_struct_fpal_PalMonsterLevelData()에 실제 스크립트 경로를 추가하세요. "
        f"(예: /Script/ProjectOpenWorld.PalMonsterLevelData)"
    )


def load_or_create_datatable(dt_folder: str, dt_name: str) -> unreal.DataTable:
    ensure_directory(dt_folder)

    dt_asset_path = f"{dt_folder}/{dt_name}"
    if does_asset_exist(dt_asset_path):
        dt = load_asset(dt_asset_path)
        if not isinstance(dt, unreal.DataTable):
            raise RuntimeError(f"경로에 DataTable이 아닌 에셋이 존재합니다: {dt_asset_path}")
        return dt

    # 없으면 생성
    row_struct = resolve_row_struct_fpal_attackdatatable()
    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", row_struct)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    dt = asset_tools.create_asset(dt_name, dt_folder, unreal.DataTable, factory)
    if not dt:
        raise RuntimeError(f"DataTable 생성 실패: {dt_asset_path}")
    save_asset(dt)
    return dt

def make_soft_object_entry(object_path: str) -> dict:
    # UPROPERTY가 TSoftObjectPtr<UAnimMontage> 또는 FSoftObjectPath 류일 때 통하는 포맷
    return {"AssetPathName": object_path, "SubPathString": ""}

def set_LevelDefault(dt: unreal.DataTable) -> bool:
    """

    """
    rows = _export_dt_rows_as_list(dt)

    # row 0/1 보장 (이름은 Row0/Row1로 신규 생성)
    if len(rows) != 0:
        return True

    rows.append({"Name": "1"})
    row = rows[0]

    # AttackData 이미 있으면 스킵
    current_attackdata = row2.get("AttackData", [])
    if isinstance(current_attackdata, list) and len(current_attackdata) > 0:
        unreal.log_warning("[DT] 2번째 Row의 AttackData에 이미 데이터가 존재합니다. 요구사항에 따라 수정/추가하지 않습니다.")
        return False

    # AttackSlot 설정 (문자열 Default 우선, 실패 시 fill 단계에서 1로 fallback)
    row2["AttackSlot"] = "Default"  # ESubAttackType::Default 기대

    # AttackData 포맷: SoftObjectPath dict (일반적으로 TSoftObjectPtr/SoftObjectPath에 적합)
   
    row2["AttackData"] = montage_object_paths
    
    ok = _fill_dt_from_rows_list(dt, rows)
    if not ok:
        raise RuntimeError("[DT] fill_from_json_string 실패. AttackData 요소 타입/구조(배열 원소 타입)가 현재 포맷과 다를 가능성이 큽니다.")

    save_asset(dt)
    unreal.log(f"[DT] 2번째 Row 업데이트 완료: AttackData={len(montage_object_paths)}개")
    return True


# -----------------------------
# 메인
# -----------------------------
def run_build_LevelData_and_patch_dt(Pal : str):
    PAL_NAME = Pal
    unreal.log(PAL_NAME)
    pal_root = f"{MONSTER_ROOT}/{PAL_NAME}"
    anim_folder = f"{pal_root}/Animation"
    montage_folder = f"{pal_root}/Montage"
    dt_folder = f"{pal_root}/DataTable"
    dt_name = f"DT_{PAL_NAME}_LevelData"

    # 폴더 보장
    ensure_directory(dt_folder)

    # DataTable 로드/생성
    dt = load_or_create_datatable(dt_folder, dt_name)
    dt
    


# 실행
arPal = PalConfig.get_pal_names_from_monster_root()
for Pal in arPal:
    run_build_LevelData_and_patch_dt(Pal)
