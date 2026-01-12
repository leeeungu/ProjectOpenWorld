import json
import unreal
from pathlib import Path

# 1) 입력 파일 경로
JSON_PATH = r"P:\source\Unreal\ProjectOpenWorld\Content\Building\DataTable\DT_BuildObjectDataTable_Source.json"
#/Script/Engine.DataTable'/Game/Building/DataTable/DT_BuildObjectDataTable.DT_BuildObjectDataTable'
TARGET_DT_ASSET = "/Game/Building/DataTable/DT_BuildObjectDataTable.DT_BuildObjectDataTable"  # 수정할 DataTable 에셋 경로

# 2) 뽑고 싶은 Row Key 목록
WANTED_KEYS = [
    "MapObjectId",
    "SortId",
    "BuildCapacity",
    "RequiredBuildWorkAmount",
    "AssetValue",
    "InstallNeighborThreshold",
    "bIsInstallOnlyOnBase",
    "bIsInstallOnlyInDoor",
    "bIsInstallOnlyHubAround",
    "InstallMaxNumInBaseCamp",
    "bInstallableNoObstacleFromCamera"
]

JSON_PATH = r"P:\source\Unreal\ProjectOpenWorld\Content\Item\DataTable\DT_PalStaticItemData_Source.json"
#/Script/Engine.DataTable'/Game/Item/DataTable/DT_PalStaticItemData.DT_PalStaticItemData'
TARGET_DT_ASSET = "/Game/Item/DataTable/DT_PalStaticItemData.DT_PalStaticItemData"  # 수정할 DataTable 에셋 경로
WANTED_KEYS  = [
   "OverrideName",
   "OverrideDescription",
   "IconName",
   "MaxStackCount",
   "Weight",
   "Price",
   "SortID",
   "bNotConsumed",
   "bEnableHandcraft",
   "VisualBlueprintClassName",
   "VisualBlueprintClassSoft",
   "Editor_RowNameHash"
]


JSON_PATH = r"P:\source\Unreal\ProjectOpenWorld\Content\Item\DataTable\DT_ItemRecipeDataTable_Source.json"
#/Script/Engine.DataTable'/Game/Item/DataTable/DT_PalStaticItemData.DT_PalStaticItemData'
TARGET_DT_ASSET = "/Game/Item/DataTable/DT_ItemRecipeDataTable.DT_ItemRecipeDataTable"  # 수정할 DataTable 에셋 경로
WANTED_KEYS  = [
   "Product_Id",
   "Product_Count",
   "WorkAmount",
   "UnlockItemID",
   "Editor_RowNameHash"
]


#/Script/Engine.DataTable'/Game/Pal/DataTable/DT_PalSpawnerPlacement.DT_PalSpawnerPlacement'
JSON_PATH = r"P:\source\Unreal\ProjectOpenWorld\Content\Pal\DataTable\DT_PalSpawnerPlacement_Source.json"
#/Script/Engine.DataTable'/Game/Item/DataTable/DT_PalStaticItemData.DT_PalStaticItemData'
TARGET_DT_ASSET = "/Game/Pal/DataTable/DT_PalSpawnerPlacement.DT_PalSpawnerPlacement"  # 수정할 DataTable 에셋 경로
WANTED_KEYS  = [
  "InstanceName",
  "SpawnerName",
  "Location",
  "StaticRadius",
  "SpawnerClass",
  "RespawnCoolTime"
]

def main():
    # source
    doc = json.loads(Path(JSON_PATH).read_text(encoding="utf-8"))
    rows = doc[0]["Rows"]  # {RowName: {Field:Value}}

    # target datatable
    dt = unreal.EditorAssetLibrary.load_asset(TARGET_DT_ASSET)
    if not dt:
        unreal.log_error(f"[ERR] DataTable load failed: {TARGET_DT_ASSET}")
        return

    # export target -> map
    export_str = dt.export_to_json_string()
    target_list = json.loads(export_str) if export_str else []
    target_map = {}
    for r in target_list:
        n = r.get("Name")
        if isinstance(n, str):
            payload = dict(r)
            payload.pop("Name", None)
            target_map[n] = payload

    # merge (update if exists, add if missing) with wanted keys only
    updated = 0
    added = 0
    for row_name, payload in rows.items():
        if not isinstance(payload, dict):
            continue

        patch = {k: payload[k] for k in WANTED_KEYS if k in payload}
        if not patch:
            continue

        if row_name in target_map:
            target_map[row_name].update(patch)
            updated += 1
        else:
            target_map[row_name] = patch
            added += 1

    # map -> import json
    merged_list = []
    for n, payload in target_map.items():
        r = {"Name": n}
        r.update(payload)
        merged_list.append(r)

    merged_str = json.dumps(merged_list, ensure_ascii=False)

    # fill & save
    ok = False
    try:
        if hasattr(unreal, "DataTableFunctionLibrary") and hasattr(unreal.DataTableFunctionLibrary, "fill_data_table_from_json_string"):
            ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(dt, merged_str)
        elif hasattr(dt, "fill_from_json_string"):
            try:
                ok = dt.fill_from_json_string(merged_str, dt.get_editor_property("row_struct"))
            except TypeError:
                ok = dt.fill_from_json_string(merged_str)
    except Exception as e:
        unreal.log_error(f"[ERR] Fill exception: {e}")
        return

    if not ok:
        unreal.log_error("[ERR] Fill failed")
        return

    unreal.EditorAssetLibrary.save_asset(TARGET_DT_ASSET)
    unreal.log(f"[OK] updated={updated}, added={added}")

if __name__ == "__main__":
    main()