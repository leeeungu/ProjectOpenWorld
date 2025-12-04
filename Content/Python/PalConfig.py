import unreal

# 모든 스크립트에서 공유할 기본 경로
GLOBAL_ANIM_DIR = "/Game/Pal/Model/Global/Animation"
MONSTER_ROOT = "/Game/Pal/Model/Monster"

# 기본 타겟 팔 이름 (필요 시 스크립트에서 변경 가능)


GLOBAL_AS_PREFIX  = "AS_MM_"
GLOBAL_BS_PREFIX  = "BS_MM_"
PAL_AS_PREFIX     = "AS_"
PAL_BS_PREFIX     = "BS_"
PAL_BP_PREFIX     = "Bp_"   # Bp_팰이름
from typing import Type, TypeVar

T = TypeVar("T", bound=unreal.Object)

# asset_path의 classtype 에 해당되는 Asset 찾는 함수
def find_asset(asset_path: str, classtype: Type[T]) -> T | None:
    asset = unreal.EditorAssetLibrary.load_asset(asset_path = asset_path)
    # 1. 에셋 자체가 없음
    if asset is None:
        unreal.log_warning(f"[Load Failed] 에셋을 찾을 수 없음: {asset_path}")
        return None

    # 2. 에셋은 있으나 타입이 다름
    if not isinstance(asset, classtype):
        unreal.log_warning(
            f"[Type Mismatch] 로드됨: {asset.get_class()} / 기대 타입: {classtype.__name__} / 경로: {asset_path}"
        )
        return None
    unreal.log(f"[Load 성공] : {asset.get_path_name()}")
    return asset

def duplicate_asset(  child_name   : str,  pal_folder : str,  template_bp: unreal.Object) -> unreal.Object | None:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = asset_tools.duplicate_asset(
              asset_name   = child_name,  
              package_path = pal_folder,
             original_object = template_bp,
    )
    if asset is None:
        unreal.log_error(f"[오류] BP 복사 실패: {child_name}")
        return None
    unreal.log(f"[성공] BP 복사 성공: {asset.get_path_name()}")
    return asset


"""
def main():
   pal_name  = "Anubis"
   asset_path = MONSTER_ROOT +"/" + pal_name + "/Mesh" + "/SK_"+ pal_name + "_Skeleton"
   Mesh = find_asset( asset_path, unreal.Skeleton)
   if Mesh:
       unreal.log(unreal.EditorAssetLibrary.get_path_name_for_loaded_asset(Mesh))

if __name__ == "__main__":
    main()
"""