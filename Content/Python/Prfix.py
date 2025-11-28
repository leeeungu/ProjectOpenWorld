import unreal

MONSTER_ROOT = "/Game/Pal/Model/Monster"

# 행동 이름에서 두 토큰을 묶어야 하는 suffix들
SUFFIX_2WORD = ["Start", "Loop", "End", "Stop", "Finish"]

def process_animation_folder(anim_path: str, pal_name: str) -> int:
    unreal.log(f"----------------------------------------------")
    unreal.log(f"[처리 시작] {anim_path} (PalName = {pal_name})")
    unreal.log(f"----------------------------------------------")

    asset_paths = unreal.EditorAssetLibrary.list_assets(
        anim_path,
        recursive=True,
        include_folder=False
    )

    changed = 0

    for asset_path in asset_paths:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)

        # AnimSequence만 처리
        if not isinstance(asset, unreal.AnimSequence):
            continue

        old_name = asset.get_name()
        old_path = asset.get_path_name()

        # 이미 AS_규칙이면 스킵
        #if old_name.startswith(f"AS_{pal_name}_"):
        #   continue

        # '_'가 아예 없으면 스킵
        if "_" not in old_name:
            unreal.log_warning(f"[SKIP '_' 없음] {old_name}")
            continue

        parts = old_name.split("_")

        # 기본은 마지막 토큰
        action = parts[-1]

        # 마지막 토큰이 Start/Loop/End/Stop/Finish 이고, 토큰이 2개 이상이면
        if len(parts) >= 2 and parts[-1] in SUFFIX_2WORD:
            action = parts[-2] + "_" + parts[-1]   # 예: Attack_Start

        new_name = f"AS_{pal_name}_{action}"
        new_path = f"{anim_path}/{new_name}"

        if old_name == new_name:
            continue

        if unreal.EditorAssetLibrary.does_asset_exist(new_path):
            unreal.log_warning(f"[충돌] {new_name} 이미 있음")
            continue

        if unreal.EditorAssetLibrary.rename_asset(old_path, new_path):
            unreal.log(f"[이름 변경] {old_name} → {new_name}")
            changed += 1
        else:
            unreal.log_warning(f"[RENAME 실패] {old_name}")

    return changed
