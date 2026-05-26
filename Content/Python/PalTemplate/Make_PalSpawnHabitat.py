"""
DT_PalSpawnHabitat 새로 생성.
- 기존 에셋이 있으면 삭제 후 새로 생성 (stale row 누적 방지)
- 펠별 서식지 룰 1행씩
"""

import unreal
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    load_or_create_datatable,
    Apply_dt_from_rows_list,
)

DT_FOLDER  = "/Game/Pal/DataTable"
DT_NAME    = "DT_PalSpawnHabitat"
ROW_STRUCT = "PalSpawnHabitatRow"

# 지형 Z 가이드 (실효 ±14,000 cm 기준)
#   0 ~ 1500     해변/얕은 지대
#   1500 ~ 5000  평지/초원
#   5000 ~ 9000  언덕
#   9000+        산악
HABITAT_DATA: dict[str, dict] = {
    # --- 물가 ---
    "Penguin":        {"min": -1000, "max":  1500, "slope": 20, "density": 5.0,  "radius": 15000, "respawn":   90, "count": 3},
    "BluePlatypus":   {"min": -1000, "max":  1500, "slope": 20, "density": 4.0,  "radius": 15000, "respawn":  120, "count": 3},
    "CaptainPenguin": {"min":  -800, "max":   800, "slope": 15, "density": 0.5,  "radius": 25000, "respawn":  600, "count": 1},

    # --- 숲/나무 ---
    "Carbunclo":      {"min":  1500, "max":  6500, "slope": 35, "density": 3.0,  "radius": 15000, "respawn":  240, "count": 2},
    "FlowerDinosaur": {"min":  1500, "max":  7000, "slope": 30, "density": 2.0,  "radius": 18000, "respawn":  300, "count": 2},
    "Monkey":         {"min":  1500, "max":  7000, "slope": 40, "density": 4.0,  "radius": 15000, "respawn":  150, "count": 3},

    # --- 보스 ---
    "Anubis":         {"min":  8000, "max": 14000, "slope": 30, "density": 0.05, "radius": 50000, "respawn": 1800, "count": 1},

    # --- 기타 ---
    "PinkCat":        {"min":  -500, "max":  4000, "slope": 40, "density": 6.0,  "radius": 12000, "respawn":   90, "count": 4},
    "SheepBall":      {"min":     0, "max":  5000, "slope": 35, "density": 5.0,  "radius": 15000, "respawn":  150, "count": 3},
    "ElecCat":        {"min":  1000, "max":  8000, "slope": 40, "density": 3.0,  "radius": 18000, "respawn":  180, "count": 2},
    "FlameBambi":     {"min":  2000, "max":  9000, "slope": 35, "density": 3.0,  "radius": 18000, "respawn":  300, "count": 2},
    "Kitsunebi":      {"min":  4000, "max": 12000, "slope": 35, "density": 1.0,  "radius": 20000, "respawn":  360, "count": 2},
}


def build_row(pal: str, h: dict) -> dict | None:
    dt_path = f"/Game/Pal/Model/Monster/{pal}/DataTable/DT_{pal}_MonsterData"
    if not unreal.EditorAssetLibrary.does_asset_exist(dt_path):
        unreal.log_warning(f"[SKIP] {pal}: {dt_path} 없음")
        return None

    return {
        "Name":            pal,
        "SpawnerDt":       f"DataTable'{dt_path}.DT_{pal}_MonsterData'",
        "MinHeight":       float(h["min"]),
        "MaxHeight":       float(h["max"]),
        "MaxSlope":        float(h["slope"]),
        "SectionDensity":  float(h["density"]),
        "StaticRadius":    float(h["radius"]),
        "RespawnCoolTime": float(h["respawn"]),
        "SpawnCount":      int(h["count"]),
    }


def force_create_datatable(folder: str, name: str, struct_name: str) -> unreal.DataTable:
    """기존 DT 가 있으면 삭제 후 새로 생성. 에디터에서 열려있으면 실패할 수 있음."""
    asset_path = f"{folder}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log(f"  기존 {name} 삭제 시도")
        if not unreal.EditorAssetLibrary.delete_asset(asset_path):
            raise RuntimeError(
                f"기존 DT 삭제 실패: {asset_path} — 에디터에서 해당 에셋을 닫고 다시 실행하세요."
            )
    return load_or_create_datatable(folder, name, struct_name)


def main():
    unreal.log("[Python] Start : DT_PalSpawnHabitat 새로 생성")
    dt = force_create_datatable(DT_FOLDER, DT_NAME, ROW_STRUCT)

    rows: list[dict] = []
    for pal, h in HABITAT_DATA.items():
        row = build_row(pal, h)
        if row:
            rows.append(row)
            unreal.log(f"  - {pal:<16} Z[{int(h['min']):>6} ~ {int(h['max']):>6}]  slope<{h['slope']:>2}  density={h['density']}")

    if not Apply_dt_from_rows_list(dt, rows):
        unreal.log_error("[ERR] DT fill 실패")
        return

    unreal.EditorAssetLibrary.save_loaded_asset(dt)
    unreal.log(f"[Python] End : {DT_NAME} 총 {len(rows)} rows 작성")


if __name__ == "__main__":
    main()
