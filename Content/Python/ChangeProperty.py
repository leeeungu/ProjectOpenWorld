import unreal
import importlib
import PalConfig
importlib.reload(PalConfig)
from PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    PAL_BP_PREFIX,
    CONFIG_PAL_NAME
)

# ==== 설정 ====

def set_default_if_none(path: str, TEMPLATE_NAME : str) -> bool:
    PROPERTY_NAME = f"{TEMPLATE_NAME}Name"   # C++ UPROPERTY 이름(정확히)
    cls = unreal.EditorAssetLibrary.load_blueprint_class(path)
    cdo = unreal.get_default_object(cls)  
    cur = cdo.get_editor_property(PROPERTY_NAME)
    
    if not cur:
        return False

    pa_name = CONFIG_PAL_NAME
    cdo.set_editor_property(PROPERTY_NAME, unreal.Name(pa_name))
    unreal.log(f"[SET] {PROPERTY_NAME} = {pa_name}")
    return True

def ChangeAndSaveAsset(TEMPLATE_NAME: str) -> bool:
    target_name = f"{PAL_BP_PREFIX}{CONFIG_PAL_NAME}_{TEMPLATE_NAME}"
    path = f"{MONSTER_ROOT}/{CONFIG_PAL_NAME}/{target_name}"
    if not target_name in path:
       return False
    if set_default_if_none(path, TEMPLATE_NAME):
       unreal.EditorAssetLibrary.save_asset(path)
       unreal.log(f"[Path] {path}")
       return True
    return False

