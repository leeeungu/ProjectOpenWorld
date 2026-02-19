import unreal
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    CONFIG_PAL_NAME
)

# ==========================================================
#  수정하고 싶은 폴더 경로 + 제거할 PREFIX 입력
# ==========================================================
#/Script/Engine.AnimSequence'/Game/Player/Character/Animation/Frank_RPG_GreatSword_Combo01_All.Frank_RPG_GreatSword_Combo01_All'
#ANIM_DIR = "/Game/Pal/Model/Monster/Kitsunebi/Animation"   # ← 원하는 경로
#REMOVE_PREFIX = "SK_Kitsunebi_Anim_Armature_" # ← 지우고 싶던 prefix
#FlowerDinosaur_Anim_Armature_AS_FlowerDinosaur_Carrying
PREFIX  = ""  # ← 추가하고 싶은 prefix

# ==========================================================
#  유틸
# ==========================================================

ANIM_DIR = f"/Game/Pal/Model/Monster/{CONFIG_PAL_NAME}/Animation"   # ← 원하는 경로
REMOVE_PREFIX = f"SK_{CONFIG_PAL_NAME}_Anim_Armature_" # ← 지우고 싶은  prefix


editor_lib = unreal.EditorAssetLibrary


def remove_prefix_from_assets(anim_dir: str, prefix: str):
    unreal.log(f"==============================================")
    unreal.log(f"[PREFIX 제거 시작] DIR = {anim_dir} / PREFIX = '{prefix}'")
    unreal.log(f"==============================================")

    asset_paths = editor_lib.list_assets(anim_dir, recursive=True, include_folder=False)

    if not asset_paths:
        unreal.log_warning(f"[WARN] 에셋 없음: {anim_dir}")
        return

    changed = 0

    for asset_path in asset_paths:
        asset = editor_lib.load_asset(asset_path.split('.')[0])
        if not asset:
            continue

        old_name = asset.get_name()
        old_full_path = asset.get_path_name()

        # prefix 없으면 스킵
        if not old_name.startswith(prefix):
            continue

        new_name = PREFIX + old_name[len(prefix):]  # prefix 제거
        new_path = f"{asset_path.rsplit('/',1)[0]}/{new_name}"

        # 이미 존재하면 충돌이므로 스킵
        if editor_lib.does_asset_exist(new_path):
            unreal.log_warning(f"[충돌] {new_name} 이미 존재 → 스킵")
            continue

        # rename
        ok = editor_lib.rename_asset(old_full_path, new_path)
        if ok:
            changed += 1
            unreal.log(f"[이름 변경] {old_name} → {new_name}")
        else:
            unreal.log_warning(f"[RENAME 실패] {old_name}")
        

    unreal.log(f"----------------------------------------------")
    unreal.log(f"[완료] 변경된 에셋 수 : {changed}")
    unreal.log(f"----------------------------------------------")



# ==========================================================
# main
# ==========================================================

def main():
    unreal.log(f"[Python] Start : 펠 네임 {CONFIG_PAL_NAME} AnimSequence 이름 변경")
    remove_prefix_from_assets(ANIM_DIR, REMOVE_PREFIX)
    unreal.log(f"[Python] End : 펠 네임 {CONFIG_PAL_NAME} AnimSequence 이름 변경")


if __name__ == "__main__":
    main()
