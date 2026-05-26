import unreal
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    MONSTER_ROOT,
    PAL_BP_PREFIX,
    load_or_create_datatable,
    Apply_dt_from_rows_list,
    get_pal_names_from_monster_root,
)

# ==========================================================
# 설정
# ==========================================================

MAX_LEVEL = 50

LEVEL_STRUCT_NAME  = "PalMonsterLevelData"
MASTER_STRUCT_NAME = "PalMonsterData"

# 스탯별 레벨당 성장률.  값(L) = Base * (1 + (L-1) * growth)
# EStatusType 의 enum 이름과 1:1.  미지정 스탯은 성장 0 (base 그대로 유지).
STAT_GROWTH = {
    "HP":             0.05,
    "MP":             0.04,
    "Stamina":        0.03,
    "Attack":         0.04,
    "Defense":        0.03,
    "Speed":          0.01,
    "MoveSpeed":      0.0,    # 캐릭터 기동력은 레벨 무관
    "Architecture":   0.03,
    "Mining":         0.03,
    "Deforest":       0.03,
    "TransportSpeed": 0.02,
}

# 펠별 Lv1 베이스. 키는 EStatusType enum 이름.
# 안 적은 스탯은 TMap 에 안 들어감 (= 해당 펠은 그 능력 없음).
PAL_STATS = {
    "PinkCat":        {"HP":  80, "MP":  20, "MoveSpeed": 450, "Stamina":  80, "Attack":  8, "Defense":  2, "Speed": 10},
    "Penguin":        {"HP":  90, "MP":  25, "MoveSpeed": 400, "Stamina":  90, "Attack":  7, "Defense":  3, "Speed": 10, "TransportSpeed": 1},
    "BluePlatypus":   {"HP": 100, "MP":  30, "MoveSpeed": 480, "Stamina": 100, "Attack":  9, "Defense":  4, "Speed": 11, "Mining": 1},
    "SheepBall":      {"HP": 150, "MP":  40, "MoveSpeed": 480, "Stamina": 120, "Attack": 12, "Defense":  6, "Speed": 12, "Deforest": 1},
    "Monkey":         {"HP": 140, "MP":  50, "MoveSpeed": 550, "Stamina": 130, "Attack": 15, "Defense":  5, "Speed": 14, "Architecture": 1},
    "ElecCat":        {"HP": 130, "MP":  60, "MoveSpeed": 580, "Stamina": 110, "Attack": 18, "Defense":  4, "Speed": 15, "Mining": 2},
    "Carbunclo":      {"HP": 160, "MP":  80, "MoveSpeed": 500, "Stamina": 140, "Attack": 20, "Defense":  7, "Speed": 14, "Architecture": 2},
    "FlameBambi":     {"HP": 220, "MP": 100, "MoveSpeed": 550, "Stamina": 180, "Attack": 26, "Defense":  8, "Speed": 16, "Deforest": 3},
    "FlowerDinosaur": {"HP": 280, "MP":  90, "MoveSpeed": 480, "Stamina": 220, "Attack": 28, "Defense": 12, "Speed": 13, "Architecture": 3},
    "Kitsunebi":      {"HP": 350, "MP": 150, "MoveSpeed": 600, "Stamina": 250, "Attack": 38, "Defense": 14, "Speed": 18, "Mining": 3},
    "CaptainPenguin": {"HP": 450, "MP": 180, "MoveSpeed": 500, "Stamina": 300, "Attack": 42, "Defense": 20, "Speed": 16, "TransportSpeed": 4},
}

# 펠별 스폰 정보 (FPalMonsterData)
PAL_SPAWN = {
    "PinkCat":        {"lv_min":  1, "lv_max": 12, "num_min": 2, "num_max": 5, "spawn_time":  90.0},
    "Penguin":        {"lv_min":  1, "lv_max": 10, "num_min": 2, "num_max": 5, "spawn_time":  90.0},
    "BluePlatypus":   {"lv_min":  3, "lv_max": 15, "num_min": 1, "num_max": 4, "spawn_time": 120.0},
    "SheepBall":      {"lv_min":  8, "lv_max": 22, "num_min": 1, "num_max": 3, "spawn_time": 150.0},
    "Monkey":         {"lv_min": 10, "lv_max": 25, "num_min": 2, "num_max": 4, "spawn_time": 150.0},
    "ElecCat":        {"lv_min": 12, "lv_max": 28, "num_min": 1, "num_max": 3, "spawn_time": 180.0},
    "Carbunclo":      {"lv_min": 15, "lv_max": 30, "num_min": 1, "num_max": 2, "spawn_time": 240.0},
    "FlameBambi":     {"lv_min": 20, "lv_max": 35, "num_min": 1, "num_max": 2, "spawn_time": 300.0},
    "FlowerDinosaur": {"lv_min": 22, "lv_max": 38, "num_min": 1, "num_max": 2, "spawn_time": 300.0},
    "Kitsunebi":      {"lv_min": 30, "lv_max": 45, "num_min": 1, "num_max": 2, "spawn_time": 360.0},
    "CaptainPenguin": {"lv_min": 35, "lv_max": 50, "num_min": 1, "num_max": 1, "spawn_time": 480.0},
}


# ==========================================================
# 함수
# ==========================================================

def build_level_rows(pal_stats: dict) -> list[dict]:
    """Lv 1~MAX_LEVEL 의 FPalMonsterLevelData row.
    각 row 의 LevelStatusData = TMap<EStatusType, double>.
    JSON 직렬화 시 enum key 는 entry 이름 그대로 ("HP", "Attack" ...).
    """
    rows = []
    for level in range(1, MAX_LEVEL + 1):
        status_map = {}
        for stat_name, base in pal_stats.items():
            growth = STAT_GROWTH.get(stat_name, 0.0)
            status_map[stat_name] = round(base * (1 + (level - 1) * growth), 2)
        rows.append({
            "Name": str(level),
            "LevelStatusData": status_map,
        })
    return rows


def build_master_row(pal_name: str, spawn: dict, level_dt: unreal.DataTable) -> dict:
    """FPalMonsterData 1행.
    - MonsterClass    : TSubclassOf<ABaseMonster>. Bp_<Pal>_Monster generated class
    - LevelDataTable  : 같이 만든 DT_<Pal>_MonsterLevelData
    """
    monster_bp_path = f"{MONSTER_ROOT}/{pal_name}/{PAL_BP_PREFIX}{pal_name}_Monster"
    monster_class = unreal.EditorAssetLibrary.load_blueprint_class(monster_bp_path)

    row = {
        "Name":        pal_name,
        "MonsterName": pal_name,
        "LvMin":       spawn["lv_min"],
        "LvMax":       spawn["lv_max"],
        "NumMin":      spawn["num_min"],
        "NumMax":      spawn["num_max"],
        "SpawnTime":   spawn["spawn_time"],
        "LevelDataTable": f"DataTable'{level_dt.get_path_name()}'",
    }

    if monster_class:
        row["MonsterClass"] = f"BlueprintGeneratedClass'{monster_class.get_path_name()}'"
        unreal.log(f"  ✓ MonsterClass = {monster_class.get_name()}")
    else:
        unreal.log_warning(
            f"[WARN] {monster_bp_path} 의 generated class 못 찾음 → MonsterClass 비움. "
            f"MakeCharacter('Monster') 가 먼저 돌았는지 확인."
        )
    return row


def make_pal_tables(pal_name: str) -> bool:
    stats = PAL_STATS.get(pal_name)
    spawn = PAL_SPAWN.get(pal_name)
    if stats is None or spawn is None:
        unreal.log_warning(f"[SKIP] '{pal_name}' PAL_STATS/PAL_SPAWN 에 없음")
        return False

    folder = f"{MONSTER_ROOT}/{pal_name}/DataTable"

    # 1) LevelData DT
    level_name = f"DT_{pal_name}_MonsterLevelData"
    level_dt = load_or_create_datatable(folder, level_name, LEVEL_STRUCT_NAME)
    if not Apply_dt_from_rows_list(level_dt, build_level_rows(stats)):
        unreal.log_error(f"[오류] {level_name} fill 실패")
        return False
    unreal.EditorAssetLibrary.save_loaded_asset(level_dt)
    unreal.log(f"[DT] {level_name}: {MAX_LEVEL} rows")

    # 2) Master DT (LevelData DT 참조)
    master_name = f"DT_{pal_name}_MonsterData"
    master_dt = load_or_create_datatable(folder, master_name, MASTER_STRUCT_NAME)
    if not Apply_dt_from_rows_list(master_dt, [build_master_row(pal_name, spawn, level_dt)]):
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
