import json
import unreal

# =========================================================
# 설정
# =========================================================

dt_path = "/Game/Item/DataTable/DT_PalItemSlotData.DT_PalItemSlotData"

# RowName에 아래 문자열들 중 하나라도 포함되면 수정
row_name_contains_list = [
    "Pickaxe",
    "gun",
    "Gun",
]

# 변경할 프로퍼티들
property_updates = {
    "bStackable": False,
    "MaxStackCount": 1,
}

ROW_NAME_KEY = "Name"


# =========================================================
# 유틸
# =========================================================

def log(msg: str):
    unreal.log(f"[DT_ROW_UPDATE] {msg}")

def load_data_table(asset_path: str) -> unreal.DataTable:
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        raise RuntimeError(f"에셋 로드 실패: {asset_path}")
    if not isinstance(asset, unreal.DataTable):
        raise RuntimeError(f"DataTable 아님: {asset_path}")
    return asset

def export_rows(dt: unreal.DataTable) -> list[dict]:
    json_str = unreal.DataTableFunctionLibrary.export_data_table_to_json_string(dt)
    if not json_str:
        raise RuntimeError("DataTable JSON export 실패")
    data = json.loads(json_str)
    if not isinstance(data, list):
        raise RuntimeError("JSON 형식이 list가 아님")
    return data

def validate_properties(sample_row: dict, updates: dict):
    for prop_name in updates.keys():
        if prop_name not in sample_row:
            raise RuntimeError(f"프로퍼티 없음: {prop_name}")

def is_row_name_matched(row_name: str, contains_list: list[str]) -> bool:
    for keyword in contains_list:
        if keyword and keyword in row_name:
            return True
    return False

def update_rows_by_name_contains_list(
    dt_path: str,
    row_name_contains_list: list[str],
    property_updates: dict,
    row_name_key: str = "Name",
):
    dt = load_data_table(dt_path)

    row_names = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
    row_name_strings = {str(name): True for name in row_names}

    rows = export_rows(dt)
    if not rows:
        raise RuntimeError("DataTable row 없음")

    validate_properties(rows[0], property_updates)

    updated_count = 0
    matched_row_names = []

    for row in rows:
        row_name = str(row.get(row_name_key, ""))

        if row_name not in row_name_strings:
            continue

        if is_row_name_matched(row_name, row_name_contains_list):
            for prop_name, prop_value in property_updates.items():
                row[prop_name] = prop_value
            updated_count += 1
            matched_row_names.append(row_name)

    if updated_count == 0:
        log(f"일치하는 row 없음. contains_list={row_name_contains_list}")
        return

    json_out = json.dumps(rows, ensure_ascii=False, indent=2)
    row_struct = unreal.DataTableFunctionLibrary.get_data_table_row_struct(dt)

    ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(
        dt,
        json_out,
        row_struct,
    )
    if not ok:
        raise RuntimeError("DataTable import 실패")

    unreal.EditorAssetLibrary.save_loaded_asset(dt)

    log(f"완료: {updated_count}개 row 수정")
    for row_name in matched_row_names:
        log(f"  - {row_name}")


# =========================================================
# 실행
# =========================================================

update_rows_by_name_contains_list(
    dt_path=dt_path,
    row_name_contains_list=row_name_contains_list,
    property_updates=property_updates,
    row_name_key=ROW_NAME_KEY,
)
