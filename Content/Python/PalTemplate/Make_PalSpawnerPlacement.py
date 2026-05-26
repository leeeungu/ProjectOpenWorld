"""
DT_PalSpawnerPlacement 자동 생성/갱신.

- 펠 목록은 MONSTER_ROOT 하위 폴더에서 자동 수집.
- 그리드+jitter 로 X,Y 분포 (셀 단위라 너무 가깝지도/멀지도 않음).
- Z 는 일단 0. 컴포넌트에 스냅이 없으므로 DT 가 미조정이면 시각적으로 바로 드러남.
- SpawnerName  = 펠 이름        (= DT_{Pal}_MonsterData 키)
- SpawnerClass = Bp_{Pal}_Monster
"""

import unreal
import math
import random
import importlib

from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    MONSTER_ROOT,
    PAL_BP_PREFIX,
    get_pal_names_from_monster_root,
    load_or_create_datatable,
    Apply_dt_from_rows_list,
    Get_MonsterBp,
)

# ==========================================================
# 설정
# ==========================================================

# 배치 영역(cm). 활성 6x6 섹션 영역(~±684,000) 보다 살짝 넓게.
WORLD_HALF_EXTENT = 1_500_000.0      # ±15 km

# 펠 1종당 기본 배치 수
DEFAULT_SPAWN_PER_PAL = 60

# 셀 한 변 대비 jitter 비율 (0=완전 격자, 1=셀 끝까지 흔들림)
JITTER_RATIO = 0.7

# 행 기본값
DEFAULT_Z                = 0.0       # 스냅이 없으니 일단 0. 디자이너가 잡거나 후속 유틸로 일괄 보정.
DEFAULT_STATIC_RADIUS    = 15000.0   # 150 m
DEFAULT_RESPAWN_COOLTIME = 60.0
DEFAULT_SPAWN_COUNT      = 3

# 펠별 오버라이드 — 보스/희귀 펠 등 따로 잡고 싶을 때만 채움
PAL_OVERRIDES: dict[str, dict] = {
    # "CaptainPenguin": {"count": 8,  "radius": 25000, "respawn": 600.0, "spawn_count": 1},
    # "PinkCat":        {"count": 100, "radius": 12000, "respawn": 45.0, "spawn_count": 4},
}

# DT
DT_FOLDER  = "/Game/Pal/DataTable"
DT_NAME    = "DT_PalSpawnerPlacement"
ROW_STRUCT = "PalSpawnerPlacementDatabaseRow"

RNG_SEED = 20260525


# ==========================================================
# 함수
# ==========================================================

def grid_jitter_points(count: int, half_extent: float, seed: int) -> list[tuple[float, float]]:
    """sqrt(count) x sqrt(count) 그리드 + 셀 내 jitter."""
    rnd  = random.Random(seed)
    side = max(1, int(math.ceil(math.sqrt(count))))
    cell = (2.0 * half_extent) / side
    half_cell = cell * 0.5
    j = cell * JITTER_RATIO * 0.5

    points: list[tuple[float, float]] = []
    for iy in range(side):
        for ix in range(side):
            cx = -half_extent + half_cell + ix * cell
            cy = -half_extent + half_cell + iy * cell
            points.append((cx + rnd.uniform(-j, j), cy + rnd.uniform(-j, j)))
            if len(points) >= count:
                return points
    return points


def build_rows_for_pal(pal: str, base_seed: int) -> list[dict]:
    cfg = PAL_OVERRIDES.get(pal, {})
    count       = cfg.get("count",       DEFAULT_SPAWN_PER_PAL)
    radius      = cfg.get("radius",      DEFAULT_STATIC_RADIUS)
    respawn     = cfg.get("respawn",     DEFAULT_RESPAWN_COOLTIME)
    spawn_count = cfg.get("spawn_count", DEFAULT_SPAWN_COUNT)

    bp = Get_MonsterBp(pal)
    if not bp:
        unreal.log_warning(f"[SKIP] {pal}: Bp_{pal}_Monster 못 찾음")
        return []

    _class = bp.get_class()


    # 펠 별 시드 분리 — 펠들끼리 같은 좌표에 안 겹치도록
    name_hash = sum(ord(c) for c in pal)
    pal_seed  = (base_seed * 1315423911) ^ (name_hash * 2654435761) & 0xFFFFFFFF
    points    = grid_jitter_points(count, WORLD_HALF_EXTENT, pal_seed)

    rows: list[dict] = []
    for idx, (x, y) in enumerate(points):
        rows.append({
            "Name":            f"{pal}_{idx:04d}",
            "InstanceName":    f"BP_PalSpawner_{pal}_{idx:04d}",
            "SpawnerName":     pal,
            "Location":        {"X": x, "Y": y, "Z": DEFAULT_Z},
            "StaticRadius":    radius,
            "SpawnerClass":    _class,
            "RespawnCoolTime": respawn,
            "SpawnCount":      spawn_count,
        })
    return rows


def main():
    unreal.log("[Python] Start : DT_PalSpawnerPlacement 생성")
    dt = load_or_create_datatable(DT_FOLDER, DT_NAME, ROW_STRUCT)

    all_rows: list[dict] = []
    for pal in get_pal_names_from_monster_root():
        rows = build_rows_for_pal(pal, RNG_SEED)
        unreal.log(f"  - {pal}: {len(rows)} rows")
        all_rows.extend(rows)

    if not Apply_dt_from_rows_list(dt, all_rows):
        unreal.log_error("[ERR] DT fill 실패")
        return

    unreal.EditorAssetLibrary.save_loaded_asset(dt)
    unreal.log(f"[Python] End : {DT_NAME} 총 {len(all_rows)} rows 작성")


if __name__ == "__main__":
    main()
