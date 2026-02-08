import json
import unreal
from pathlib import Path

#/Script/Engine.DataTable'/Game/Pal/DataTable/DT_PalCharacterIconDataTable.DT_PalCharacterIconDataTable'
JSON_PATH = r"P:\source\Unreal\ProjectOpenWorld\Content\Pal\DataTable\DT_PalCharacterIconDataTable.json"
#/Script/Engine.DataTable'/Game/Building/DataTable/DT_BuildObjectIconDataTable.DT_BuildObjectIconDataTable'
TARGET_DT_ASSET = r"/Game/Pal/DataTable/DT_PalCharacterIconDataTable.DT_PalCharacterIconDataTable"

WANTED_KEYS = ["Icon"]

def main():
    # source rows
    doc = json.loads(Path(JSON_PATH).read_text(encoding="utf-8"))
    src_rows = doc[0]["Rows"]

    # target datatable
    dt = unreal.EditorAssetLibrary.load_asset(TARGET_DT_ASSET)
    if not dt:
        unreal.log_error(f"[ERR] load failed: {TARGET_DT_ASSET}")
        return

    # export target -> map (RowName -> payload)
    export_str = dt.export_to_json_string() or "[]"
    target_list = json.loads(export_str)
    target_map = {}
    for r in target_list:
        n = r.get("Name")
        if isinstance(n, str):
            payload = dict(r)
            payload.pop("Name", None)
            target_map[n] = payload

    updated = 0
    added = 0
    missing_asset = 0

    for row_name, payload in src_rows.items():
        if not isinstance(payload, dict):
            continue

        patch = {}
        for k in WANTED_KEYS:
            if k not in payload:
                continue

            v = payload[k]

            # SoftObjectPath JSON 형태면 AssetPathName을 에디터에서 resolve
            if isinstance(v, dict) and "AssetPathName" in v:
                ap = v.get("AssetPathName", "")
                sp = v.get("SubPathString", "")
                
                # "Texture2D'/Game/.../X.X'" 같은 형태면 따옴표 내부만 추출
                if isinstance(ap, str) and "'" in ap:
                    parts = ap.split("'")
                    if len(parts) >= 2:
                        ap = parts[1]

                resolved = ap
                if isinstance(ap, str) and ap:
                    obj = unreal.EditorAssetLibrary.load_asset(ap)
                    if obj:
                        resolved = obj.get_path_name()  # 정규 ObjectPath로 저장
                        #patch[k] = obj
                    else:
                        missing_asset += 1

                patch[k] = ap #{"AssetPathName": resolved, "SubPathString": sp if isinstance(sp, str) else ""}
                #print(patch[k])
            else:
                patch[k] = v
            
        if not patch:
            continue

        if row_name in target_map:
            target_map[row_name].update(patch)
            updated += 1
        else:
            target_map[row_name] = patch
            added += 1
        

    # map -> import json list
    merged_list = []
    for n, payload in target_map.items():
        r = {"Name": n}
        r.update(payload)
        merged_list.append(r)

    merged_str = json.dumps(merged_list, ensure_ascii=False)

    ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(dt, merged_str)
    if not ok:
        unreal.log_error("[ERR] fill failed (check Output Log)")
        return

    unreal.EditorAssetLibrary.save_asset(TARGET_DT_ASSET)
    unreal.log(f"[OK] updated={updated}, added={added}, missing_asset={missing_asset}")

if __name__ == "__main__":
    main()
