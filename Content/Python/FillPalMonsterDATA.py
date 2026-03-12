import unreal
import json
import importlib

def load_or_create_datatable(dt_folder: str, dt_name: str, structStr : str) -> unreal.DataTable:
    unreal.log("Find Folder")
    if not unreal.EditorAssetLibrary.does_directory_exist(dt_folder):
        unreal.EditorAssetLibrary.make_directory(dt_folder)    
    dt_asset_path = f"{dt_folder}/{dt_name}"
    if does_asset_exist(dt_asset_path):
        dt = load_asset(dt_asset_path)
        if not isinstance(dt, unreal.DataTable):
            raise RuntimeError(f"경로에 DataTable이 아닌 에셋이 존재합니다: {dt_asset_path}")
        return dt
    unreal.log("Find Struct")
    candidates = [
        f"/Script/ProjectOpenWorld.{structStr}",
        structStr,  # find_object fallback 용
    ]

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
            f"RowStruct 'FPalMonsterLevelData'을 찾지 못했습니다. "
            f"resolve_row_struct_fpal_PalMonsterLevelData()에 실제 스크립트 경로를 추가하세요. "
            f"(예: /Script/ProjectOpenWorld.PalMonsterLevelData)"
        )

    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", row_struct)
    unreal.log("make Struct")
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    dt = asset_tools.create_asset(dt_name, dt_folder, unreal.DataTable, factory)
    if not dt:
        raise RuntimeError(f"DataTable 생성 실패: {dt_asset_path}")
    save_asset(dt)
    return dt

unreal.log("Fill DT")
load_or_create_datatable("/Game/Pal/Model/Global",  "DT_TestData", "PalLevelDataTable")

