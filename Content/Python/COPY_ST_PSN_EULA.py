import unreal
import os
import json
import csv

def _ensure_content_directory(game_path: str) -> None:
    # /Game/AAA/BBB 형태의 디렉터리 생성 보장
    if not game_path.startswith("/Game"):
        raise ValueError('destination_path는 반드시 "/Game/..." 형식이어야 합니다.')
    unreal.EditorAssetLibrary.make_directory(game_path)

def _make_temp_csv_path(filename: str) -> str:
    # Intermediate 하위에 임시 파일 생성 (절대경로 + 슬래시 정규화)
    base = unreal.Paths.project_intermediate_dir()
    tmp_dir = unreal.Paths.combine([base, "TmpStringTableImport"])
    tmp_dir = unreal.Paths.convert_relative_path_to_full(tmp_dir)
    tmp_dir = unreal.Paths.normalize_directory_name(tmp_dir)

    os.makedirs(tmp_dir, exist_ok=True)

    csv_path = unreal.Paths.combine([tmp_dir, filename])
    csv_path = unreal.Paths.convert_relative_path_to_full(csv_path)
    csv_path = unreal.Paths.normalize_filename(csv_path)
    return csv_path

def import_stringtables_from_json_file(
    json_file_path: str,
    destination_path: str,
    replace_existing: bool = True,
    cleanup_temp_csv: bool = True,
) -> list[str]:
    """
    json_file_path: StringTable Export 형태 JSON 파일 경로
    destination_path: 생성할 StringTable 에셋 위치 (/Game/...)
    replace_existing: 동일 이름 에셋이 있으면 덮어쓰기
    cleanup_temp_csv: 임포트 후 임시 CSV 삭제
    return: 생성/갱신된 에셋 경로 리스트
    """

    if not os.path.isfile(json_file_path):
        raise FileNotFoundError(f"JSON 파일이 존재하지 않음: {json_file_path}")

    _ensure_content_directory(destination_path)

    # UTF-8 BOM까지 대응
    with open(json_file_path, "r", encoding="utf-8-sig") as f:
        data = json.load(f)

    if isinstance(data, dict):
        data = [data]
    if not isinstance(data, list):
        raise ValueError("JSON 최상단은 dict 또는 list여야 합니다.")

    blocks = [x for x in data if isinstance(x, dict) and x.get("Type") == "StringTable"]
    if not blocks:
        raise ValueError("Type == 'StringTable' 블록이 JSON에 없습니다.")

    created_assets: list[str] = []

    for block in blocks:
        asset_name = block.get("Name") or (block.get("StringTable", {}).get("TableNamespace")) or "ST_Imported"
        st = block.get("StringTable", {})
        entries = st.get("KeysToEntries", {})

        if not isinstance(entries, dict) or len(entries) == 0:
            unreal.log_warning(f"[StringTableImport] '{asset_name}' KeysToEntries가 비어있음 - 스킵")
            continue

        # 1) 임시 CSV 생성 (UE StringTable 임포터가 이해하는 포맷: Key,SourceString)
        tmp_csv = _make_temp_csv_path(f"__tmp_{asset_name}.csv")

        # CSV 파싱 안정성 위해 QUOTE_ALL + utf-8-sig
        with open(tmp_csv, "w", encoding="utf-8-sig", newline="") as fp:
            writer = csv.writer(fp, quoting=csv.QUOTE_ALL)
            writer.writerow(["Key", "SourceString"])

            # 키 정렬은 diff/재현성 확보용
            for k in sorted(entries.keys(), key=lambda x: str(x)):
                v = entries.get(k, "")
                key = "" if k is None else str(k)
                val = "" if v is None else str(v)

                # (선택) 값에 개행이 들어가 UE 임포터가 흔들리는 프로젝트가 있으면 다음 줄 사용:
                # val = val.replace("\r\n", "\n").replace("\r", "\n")

                writer.writerow([key, val])

        if not os.path.exists(tmp_csv):
            raise RuntimeError(f"임시 CSV 생성 실패: {tmp_csv}")

        # 2) StringTableFactory로 임포트 → uasset 생성/갱신
        task = unreal.AssetImportTask()
        task.automated = True
        task.filename = tmp_csv
        task.destination_path = destination_path
        task.destination_name = asset_name
        task.replace_existing = replace_existing
        task.save = True
        task.factory = unreal.StringTableFactory()

        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

        imported = list(task.imported_object_paths)
        if not imported:
            # 실패 시 경로를 바로 로그로 남겨 원인 추적 가능
            raise RuntimeError(f"StringTable 임포트 실패: asset={asset_name}, csv={tmp_csv}, dest={destination_path}")

        asset_path = imported[0]
        unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
        created_assets.append(asset_path)

        # 3) 임시 CSV 삭제
        if cleanup_temp_csv:
            try:
                os.remove(tmp_csv)
            except Exception as e:
                unreal.log_warning(f"[StringTableImport] 임시 CSV 삭제 실패: {tmp_csv} ({e})")

        unreal.log(f"[StringTableImport] OK: {asset_path}")

    return created_assets


# ===== 사용 예시 =====
# created = import_stringtables_from_json_file(
#     json_file_path=r"D:\temp\ST_PSN_EULA.json",
#     destination_path="/Game/Localization/StringTables"
# )
# unreal.log(f"Created/Updated: {created}")

#P:\FModel\Output\Exports\Pal\Content\Pal\StringTables
#/Script/Engine.StringTable'/Game/Global/StringTable/ST_Test.ST_Test'
if __name__ == "__main__":
    import_stringtables_from_json_file("P:/FModel/Output/Exports/Pal/Content/Pal/StringTables/ST_PSN_EULA.json", "/Game/Global/StringTable/")

# ===== 사용 예시 =====
# 1) JSON 파일이 있을 때
# import_string_table_from_export_json(r"D:\temp\ST_PSN_EULA.json",
#                                     destination_path="/Game/Localization/StringTables")

# 2) JSON 문자열을 그대로 넣을 때(너가 올린 내용 그대로)
# json_text = r''' ...붙여넣기... '''
# import_string_table_from_export_json(json_text,
#                                     destination_path="/Game/Localization/StringTables")
