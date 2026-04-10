import json
import unreal

# =========================================================
# 설정
# =========================================================

from_dt_path = "/Game/Item/DataTable/DT_PalStaticItemData1.DT_PalStaticItemData1"
to_dt_path   = "/Game/Item/DataTable/DT_PalItemSlotData.DT_PalItemSlotData"

# from_struct 프로퍼티명 -> to_struct 프로퍼티명
property_mappings = [
    ("MaxStackCount", "MaxStackCount"),
    ("Weight", "Weight"),
    ("SortID", "SortID"),
]

# JSON export에서 RowName 키 이름.
# 보통 "Name"으로 동작하지만, 프로젝트별 export 결과가 다르면 여기만 수정
ROW_NAME_KEY = "Name"

# True면 from에 있고 to에 없는 row는 새로 생성
create_missing_rows = True

# True면 to에만 있는 row도 유지
keep_unmatched_destination_rows = True


# =========================================================
# 유틸
# =========================================================

def log(msg: str):
    unreal.log(f"[DT_COPY] {msg}")

def log_error(msg: str):
    unreal.log_error(f"[DT_COPY] {msg}")

def load_data_table(asset_path: str) -> unreal.DataTable:
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        raise RuntimeError(f"에셋 로드 실패: {asset_path}")
    if not isinstance(asset, unreal.DataTable):
        raise RuntimeError(f"DataTable 아님: {asset_path}")
    return asset

def export_dt_to_rows(dt: unreal.DataTable) -> list[dict]:
    json_str = unreal.DataTableFunctionLibrary.export_data_table_to_json_string(dt)
    if not json_str:
        raise RuntimeError(f"DataTable JSON export 실패: {dt.get_path_name()}")
    data = json.loads(json_str)
    if not isinstance(data, list):
        raise RuntimeError("DataTable JSON 형식이 예상과 다름: list 아님")
    return data

def rows_to_row_map(rows: list[dict], row_name_key: str) -> dict[str, dict]:
    result = {}
    for row in rows:
        if row_name_key not in row:
            raise RuntimeError(f"RowName 키 '{row_name_key}' 없음. row={row}")
        row_name = row[row_name_key]
        result[str(row_name)] = row
    return result

def validate_mapping(source_sample: dict, dest_sample: dict, mappings: list[tuple[str, str]]):
    for src_prop, dst_prop in mappings:
        if src_prop not in source_sample:
            raise RuntimeError(f"source 프로퍼티 없음: {src_prop}")
        if dst_prop not in dest_sample:
            raise RuntimeError(f"destination 프로퍼티 없음: {dst_prop}")

def copy_properties_between_rows(
    source_row_map: dict[str, dict],
    dest_row_map: dict[str, dict],
    mappings: list[tuple[str, str]],
    row_name_key: str,
    create_missing: bool,
):
    copied_count = 0
    created_count = 0

    for row_name, source_row in source_row_map.items():
        if row_name in dest_row_map:
            dest_row = dest_row_map[row_name]
        else:
            if not create_missing:
                continue
            dest_row = {row_name_key: row_name}
            dest_row_map[row_name] = dest_row
            created_count += 1

        for src_prop, dst_prop in mappings:
            if src_prop in source_row:
                dest_row[dst_prop] = source_row[src_prop]

        copied_count += 1

    return copied_count, created_count

def rebuild_destination_rows(
    original_dest_rows: list[dict],
    dest_row_map: dict[str, dict],
    row_name_key: str,
    keep_unmatched: bool,
) -> list[dict]:
    if keep_unmatched:
        # 기존 destination row 순서를 우선 유지
        ordered = []
        visited = set()

        for row in original_dest_rows:
            row_name = str(row[row_name_key])
            if row_name in dest_row_map:
                ordered.append(dest_row_map[row_name])
                visited.add(row_name)

        # 새로 생성된 row 뒤에 추가
        for row_name, row in dest_row_map.items():
            if row_name not in visited:
                ordered.append(row)

        return ordered

    # source에 의해 갱신된 row만 남김
    return list(dest_row_map.values())


# =========================================================
# 실행
# =========================================================

def copy_dt_properties(
    source_dt_path: str,
    destination_dt_path: str,
    mappings: list[tuple[str, str]],
    row_name_key: str = "Name",
    create_missing: bool = True,
    keep_unmatched: bool = True,
):
    source_dt = load_data_table(source_dt_path)
    destination_dt = load_data_table(destination_dt_path)

    source_rows = export_dt_to_rows(source_dt)
    dest_rows = export_dt_to_rows(destination_dt)

    if not source_rows:
        raise RuntimeError("source DT에 row가 없음")
    if not dest_rows and not create_missing:
        raise RuntimeError("destination DT가 비어 있고 create_missing=False 임")

    source_row_map = rows_to_row_map(source_rows, row_name_key)
    dest_row_map = rows_to_row_map(dest_rows, row_name_key) if dest_rows else {}

    source_sample = next(iter(source_row_map.values()))
    dest_sample = next(iter(dest_row_map.values())) if dest_row_map else {row_name_key: "SampleRow"}

    # destination DT가 비어 있으면 구조 검증은 source 쪽만 하고,
    # 실제 fill 단계에서 destination row struct 기준으로 import 된다.
    if dest_row_map:
        validate_mapping(source_sample, dest_sample, mappings)
    else:
        for src_prop, _ in mappings:
            if src_prop not in source_sample:
                raise RuntimeError(f"source 프로퍼티 없음: {src_prop}")

    copied_count, created_count = copy_properties_between_rows(
        source_row_map=source_row_map,
        dest_row_map=dest_row_map,
        mappings=mappings,
        row_name_key=row_name_key,
        create_missing=create_missing,
    )

    rebuilt_rows = rebuild_destination_rows(
        original_dest_rows=dest_rows,
        dest_row_map=dest_row_map,
        row_name_key=row_name_key,
        keep_unmatched=keep_unmatched,
    )

    json_out = json.dumps(rebuilt_rows, ensure_ascii=False, indent=2)

    row_struct = unreal.DataTableFunctionLibrary.get_data_table_row_struct(destination_dt)
    ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(
        destination_dt,
        json_out,
        row_struct,
    )
    if not ok:
        raise RuntimeError("destination DT fill 실패")

    unreal.EditorAssetLibrary.save_loaded_asset(destination_dt)

    log(
        f"완료: copied={copied_count}, created={created_count}, "
        f"source='{source_dt_path}', dest='{destination_dt_path}'"
    )


# 실행
copy_dt_properties(
    source_dt_path=from_dt_path,
    destination_dt_path=to_dt_path,
    mappings=property_mappings,
    row_name_key=ROW_NAME_KEY,
    create_missing=create_missing_rows,
    keep_unmatched=keep_unmatched_destination_rows,
)
