import unreal
# 모든 스크립트에서 공유할 기본 경로
GLOBAL_ANIM_DIR = "/Game/Pal/Model/Global/Animation"
MONSTER_ROOT = "/Game/Pal/Model/Monster"

# 기본 타겟 팔 이름 (필요 시 스크립트에서 변경 가능)
PAL_NAME = "Anubis"

GLOBAL_AS_PREFIX  = "AS_MM_"
GLOBAL_BS_PREFIX  = "BS_MM_"
PAL_AS_PREFIX     = "AS_"
PAL_BS_PREFIX     = "BS_"

PARENT_CLASS_PATH2   = "/Game/Pal/Map/Test/Bp_TestCreature.Bp_TestCreature_C"


"""
from PalConfig import (
    GLOBAL_ANIM_DIR,
    MONSTER_ROOT,
    PAL_NAME,
    GLOBAL_AS_PREFIX,
    GLOBAL_BS_PREFIX,
    PAL_AS_PREFIX,
    PAL_BS_PREFIX,
)


def find_skeletal_mesh(pal_folder, pal_name):
    mesh_path = f"{pal_folder}/Mesh/SK_{pal_name}"
    if editor_lib.does_asset_exist(mesh_path):
        mesh = editor_lib.load_asset(mesh_path)
        if isinstance(mesh, unreal.SkeletalMesh):
            return mesh
    unreal.log_warning(f"[Mesh 없음] {mesh_path}")
    return None

def find_AimBp(pal_folder, pal_name):
    ABP_path = f"{pal_folder}/ABP_{pal_name}"
    if editor_lib.does_asset_exist(ABP_path):
        ABP = editor_lib.load_asset(ABP_path)
        if isinstance(ABP, unreal.AnimBlueprint):
            return ABP
    unreal.log_warning(f"[ABP 없음] {ABP_path}")
    return None
"""
