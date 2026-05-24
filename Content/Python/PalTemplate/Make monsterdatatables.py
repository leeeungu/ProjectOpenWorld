import unreal
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    MONSTER_ROOT,
    Get_MonsterBp,
    load_or_create_datatable,
    Apply_dt_from_rows_list,
    get_pal_names_from_monster_root,
    PAL_BP_PREFIX
)


# ==========================================================
# 설정 — 새 펠 추가는 여기 한 줄
# ==========================================================

MAX_LEVEL = 50

# 성장 곡선: HP/Atk = Base * (1 + (L-1) * growth), Armor = Base + (L-1) * growth
HP_GROWTH    = 0.05
ATK_GROWTH   = 0.04
ARMOR_GROWTH = 0.5

LEVEL_STRUCT_NAME  = "PalMonsterLevelData"   # FPalMonsterLevelData
MASTER_STRUCT_NAME = "PalMonsterData"        # FPalMonsterData

# 펠별 베이스 스탯 + 마스터 데이터
PAL_DATA: dict[str, dict] = {
    "PinkCat":        {"hp":  80, "atk":  8, "armor":  2, "speed": 450, "lv_min":  1, "lv_max": 12, "num_min": 2, "num_max": 5, "spawn_time":  90.0},
    "Penguin":        {"hp":  90, "atk":  7, "armor":  3, "speed": 400, "lv_min":  1, "lv_max": 10, "num_min": 2, "num_max": 5, "spawn_time":  90.0},
    "BluePlatypus":   {"hp": 100, "atk":  9, "armor":  4, "speed": 480, "lv_min":  3, "lv_max": 15, "num_min": 1, "num_max": 4, "spawn_time": 120.0},
    "SheepBall":      {"hp": 150, "atk": 12, "armor":  6, "speed": 480, "lv_min":  8, "lv_max": 22, "num_min": 1, "num_max": 3, "spawn_time": 150.0},
    "Monkey":         {"hp": 140, "atk": 15, "armor":  5, "speed": 550, "lv_min": 10, "lv_max": 25, "num_min": 2, "num_max": 4, "spawn_time": 150.0},
    "ElecCat":        {"hp": 130, "atk": 18, "armor":  4, "speed": 580, "lv_min": 12, "lv_max": 28, "num_min": 1, "num_max": 3, "spawn_time": 180.0},
    "Carbunclo":      {"hp": 160, "atk": 20, "armor":  7, "speed": 500, "lv_min": 15, "lv_max": 30, "num_min": 1, "num_max": 2, "spawn_time": 240.0},
    "FlameBambi":     {"hp": 220, "atk": 26, "armor":  8, "speed": 550, "lv_min": 20, "lv_max": 35, "num_min": 1, "num_max": 2, "spawn_time": 300.0},
    "FlowerDinosaur": {"hp": 280, "atk": 28, "armor": 12, "speed": 480, "lv_min": 22, "lv_max": 38, "num_min": 1, "num_max": 2, "spawn_time": 300.0},
    "Kitsunebi":      {"hp": 350, "atk": 38, "armor": 14, "speed": 600, "lv_min": 30, "lv_max": 45, "num_min": 1, "num_max": 2, "spawn_time": 360.0},
    "CaptainPenguin": {"hp": 450, "atk": 42, "armor": 20, "speed": 500, "lv_min": 35, "lv_max": 50, "num_min": 1, "num_max": 1, "spawn_time": 480.0},
}


# ==========================================================
# 함수
# ==========================================================

def build_level_rows(base: dict) -> list[dict]:
    """Lv 1~MAX_LEVEL 의 FPalMonsterLevelData row."""
    rows = []
    for level in range(1, MAX_LEVEL + 1):
        rows.append({
            "Name": str(level),
            "MaxHP":       round(base["hp"]    * (1 + (level - 1) * HP_GROWTH)),
            "AttackPower": round(base["atk"]   * (1 + (level - 1) * ATK_GROWTH), 2),
            "Armor":       round(base["armor"] + (level - 1) * ARMOR_GROWTH, 1),
            "MoveSpeed":   float(base["speed"]),
        })
    return rows


def build_master_row(pal_name: str, base: dict, level_dt: unreal.DataTable) -> dict:
    """FPalMonsterData 1행. MonsterClass 와 LevelDataTable ref 포함."""
    monster_bp_path = f"{MONSTER_ROOT}/{pal_name}/{PAL_BP_PREFIX}{pal_name}_Monster"
    monster_class = unreal.EditorAssetLibrary.load_blueprint_class(monster_bp_path)
    row = {
        "Name":        pal_name,
        "MonsterName": pal_name,
        "MonsterClass" : monster_class,
        "LvMin":       base["lv_min"],
        "LvMax":       base["lv_max"],
        "NumMin":      base["num_min"],
        "NumMax":      base["num_max"],
        "SpawnTime":   base["spawn_time"],
        "LevelDataTable": f"DataTable'{level_dt.get_path_name()}'",
    }
    monster_bp = Get_MonsterBp(pal_name)
    if monster_bp:
        row["MonsterClass"] = f"BlueprintGeneratedClass'{monster_bp.get_path_name()}_C'"
    else:
        unreal.log_warning(f"[WARN] Bp_{pal_name}_Monster 못 찾음 → MonsterClass 비움")
    return row


def make_pal_tables(pal_name: str) -> bool:
    base = PAL_DATA.get(pal_name)
    if base is None:
        unreal.log_warning(f"[SKIP] PAL_DATA 에 '{pal_name}' 없음 — 추가 후 재실행")
        return False

    folder = f"{MONSTER_ROOT}/{pal_name}/DataTable"

    # 1) LevelData DT
    level_name = f"DT_{pal_name}_MonsterLevelData"
    level_dt = load_or_create_datatable(folder, level_name, LEVEL_STRUCT_NAME)
    if not Apply_dt_from_rows_list(level_dt, build_level_rows(base)):
        unreal.log_error(f"[오류] {level_name} fill 실패")
        return False
    unreal.EditorAssetLibrary.save_loaded_asset(level_dt)
    unreal.log(f"[DT] {level_name}: {MAX_LEVEL} rows")

    # 2) Master DT (LevelData DT 참조해야 하므로 1번 이후에 만듦)
    master_name = f"DT_{pal_name}_MonsterData"
    master_dt = load_or_create_datatable(folder, master_name, MASTER_STRUCT_NAME)
    if not Apply_dt_from_rows_list(master_dt, [build_master_row(pal_name, base, level_dt)]):
        unreal.log_error(f"[오류] {master_name} fill 실패")
        return False
    unreal.EditorAssetLibrary.save_loaded_asset(master_dt)
    unreal.log(f"[DT] {master_name}: 1 row")
    return True


# ==========================================================
# main
# ==========================================================

def main():
    unreal.log("[Python] Start : Monster DataTables 생성")
    for pal in get_pal_names_from_monster_root():
        unreal.log(f"--- {pal} ---")
        make_pal_tables(pal)
    unreal.log("[Python] End : Monster DataTables 생성 완료")


if __name__ == "__main__":
    main()
