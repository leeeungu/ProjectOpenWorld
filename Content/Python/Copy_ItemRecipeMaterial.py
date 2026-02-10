import json
import unreal
from pathlib import Path

# 1) 입력 파일 경로
JSON_PATH = r"P:\FModel\Output\Exports\Pal\Content\Pal\DataTable\Character\DT_PalDropItem.json"

#/Script/Engine.DataTable'/Game/Item/DataTable/DT_PalDropItem.DT_PalDropItem'
TARGET_DT_ASSET = "/Game/Item/DataTable/DT_PalDropItem.DT_PalDropItem"  # 수정할 DataTable 에셋 경로



def main():
    # source
    doc = json.loads(Path(JSON_PATH).read_text(encoding="utf-8"))
    src_rows = doc[0]["Rows"]  # {RowName: {필드...}}

    # target datatable
    dt = unreal.EditorAssetLibrary.load_asset(TARGET_DT_ASSET)
    if not dt:
        unreal.log_error(f"[ERR] load failed: {TARGET_DT_ASSET}")
        return

    # export target -> rowmap
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

    for row_name, payload in src_rows.items():
        if not isinstance(payload, dict):
            continue

        # ---- 핵심: Material 배열 만들기 ----
        mats = []
        for i in range(1, 10):
            ID = payload.get(f"ItemId{i}")

            # 필요 없으면 제외(권장): None/0 제거
            if ID == "None":
                continue
            mats.append({
                "ItemId": ID,
                "Rate": payload.get(f"Rate{i}"),
                "min" : payload.get(f"min{i}"),
                "Max" : payload.get(f"Max{i}")
            })
            
            #unreal.log(f"ItemID : " + ID)
        if not mats:
            continue
        patch = {
            "CharacterID" : payload.get(f"CharacterID"),
            "Level" : payload.get(f"Level"),
            "DropItemList": mats
            }

        if row_name in target_map:
            target_map[row_name].update(patch)
            updated += 1
        else:
            target_map[row_name] = patch
            added += 1

    # rowmap -> import json list
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
    unreal.log(f"[OK] updated={updated}, added={added}")

if __name__ == "__main__":
    main()
