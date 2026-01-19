# -*- coding: utf-8 -*-
import json
import math
import random
from typing import List, Tuple, Optional, Dict, Any

import unreal


# =========================
# 설정값(여기만 수정)
# =========================
#/Script/Engine.DataTable'/Game/Landscape/DT_FoliageData.DT_FoliageData'

DATATABLE_PATH = "/Game/Landscape/DT_FoliageData"  # <- 네 DataTable 에셋 경로로 변경
ROW_NAME = "TreePattern_Auto_01"
APPEND_MODE = True            # True: 기존 rows 유지 + 추가 / False: 테이블 전체를 이번 결과로 덮어쓰기
OVERWRITE_IF_EXISTS = True    # 동일 Name이 이미 있으면 덮어쓸지 여부(append일 때만 의미)
SEED = 12345

FOLIAGE_TYPES = [
    "/Script/Foliage.FoliageType_InstancedStaticMesh'/Game/LUSH_StylizedEnvironmentSet/Foliage/Trees/SMF_Tree_Medium_02.SMF_Tree_Medium_02'",
    "/Script/Foliage.FoliageType_InstancedStaticMesh'/Game/LUSH_StylizedEnvironmentSet/Foliage/Trees/SMF_Tree_Medium_01.SMF_Tree_Medium_01'",
    "/Script/Foliage.FoliageType_InstancedStaticMesh'/Game/LUSH_StylizedEnvironmentSet/Foliage/Trees/SMF_Tree_Large_01.SMF_Tree_Large_01'",
]

COUNT = 30
X_RANGE = (-20000.0, 20000.0)
Y_RANGE = (-20000.0, 20000.0)
MIN_DIST = 1000              # 너무 빽빽해지는 것 방지(0이면 제약 없음)
MODE = "rect"                 # "rect" or "disk"
DISK_RADIUS = 3200.0          # MODE="disk"일 때만 사용
KEEPOUT_RADIUS = 0.0          # 중앙 공터 반경(원하면 900~1500 등)
Z_VALUE = 0.0

# 타입 가중치(선택): foliage_types와 길이 동일해야 함
# 예: 중간나무/중간나무/큰나무 => 큰나무 출현을 낮추고 싶으면 0.1 등
TYPE_WEIGHTS = [0.45, 0.45, 0.10]


# =========================
# 생성 로직
# =========================
def _rand_in_rect(rng: random.Random, x_range: Tuple[float, float], y_range: Tuple[float, float]) -> Tuple[float, float]:
    return rng.uniform(x_range[0], x_range[1]), rng.uniform(y_range[0], y_range[1])


def _rand_in_disk(rng: random.Random, radius: float) -> Tuple[float, float]:
    u = rng.random()
    r = math.sqrt(u) * radius
    theta = rng.uniform(0.0, math.tau)
    return r * math.cos(theta), r * math.sin(theta)


def _is_far_enough(x: float, y: float, placed: List[Tuple[float, float]], min_dist_sq: float) -> bool:
    for px, py in placed:
        dx = x - px
        dy = y - py
        if (dx * dx + dy * dy) < min_dist_sq:
            return False
    return True


def generate_foliage_row(
    name: str,
    foliage_types: List[str],
    count: int,
    x_range: Tuple[float, float],
    y_range: Tuple[float, float],
    min_dist: float,
    seed: Optional[int],
    mode: str,
    disk_radius: float,
    keepout_radius: float,
    z_value: float,
    type_weights: Optional[List[float]],
    max_attempts_per_point: int = 80,
) -> Dict[str, Any]:
    if not foliage_types:
        raise ValueError("FOLIAGE_TYPES가 비어 있습니다.")
    if count <= 0:
        raise ValueError("COUNT는 1 이상이어야 합니다.")
    if mode not in ("rect", "disk"):
        raise ValueError("MODE는 'rect' 또는 'disk' 여야 합니다.")
    if type_weights is not None and len(type_weights) != len(foliage_types):
        raise ValueError("TYPE_WEIGHTS 길이는 FOLIAGE_TYPES와 같아야 합니다.")

    rng = random.Random(seed)
    placed_xy: List[Tuple[float, float]] = []
    min_dist_sq = min_dist * min_dist
    keepout_sq = keepout_radius * keepout_radius

    def pick_type() -> str:
        if type_weights is None:
            return rng.choice(foliage_types)
        return rng.choices(foliage_types, weights=type_weights, k=1)[0]

    instance_data: List[Dict[str, Any]] = []
    for _ in range(count):
        chosen_type = pick_type()

        accepted = False
        cx = cy = 0.0

        for _try in range(max_attempts_per_point):
            if mode == "rect":
                x, y = _rand_in_rect(rng, x_range, y_range)
            else:
                x, y = _rand_in_disk(rng, disk_radius)

            if keepout_radius > 0.0 and (x * x + y * y) < keepout_sq:
                continue

            if min_dist > 0.0 and not _is_far_enough(x, y, placed_xy, min_dist_sq):
                continue

            cx, cy = x, y
            accepted = True
            break

        # 실패 시: min_dist가 과도한 경우가 대부분 → 최종 1회 강제 배치(품질보다 진행 우선)
        if not accepted:
            if mode == "rect":
                cx, cy = _rand_in_rect(rng, x_range, y_range)
            else:
                cx, cy = _rand_in_disk(rng, disk_radius)

        placed_xy.append((cx, cy))
        instance_data.append({
            "FoliageType": chosen_type,
            "InstanceOffset": {"X": round(cx, 3), "Y": round(cy, 3), "Z": round(z_value, 3)},
        })

    return {"Name": name, "InstanceData": instance_data}


# =========================
# DataTable I/O
# =========================
def _load_datatable(dt_path: str) -> unreal.DataTable:
    dt = unreal.EditorAssetLibrary.load_asset(dt_path)
    if not dt:
        raise RuntimeError(f"DataTable 로드 실패: {dt_path}")
    if not isinstance(dt, unreal.DataTable):
        raise RuntimeError(f"에셋이 DataTable이 아님: {dt_path} ({type(dt)})")
    return dt


def _read_rows_as_json_list(dt: unreal.DataTable) -> List[Dict[str, Any]]:
    # UE API: export_to_json_string() :contentReference[oaicite:1]{index=1}
    s = dt.export_to_json_string()
    if not s:
        return []
    try:
        rows = json.loads(s)
        if isinstance(rows, list):
            return rows
        return []
    except Exception:
        # JSON 파싱 실패 시: 빈 테이블로 간주
        return []


def _make_unique_name(existing: set, base: str) -> str:
    if base not in existing:
        return base
    i = 1
    while True:
        cand = f"{base}_{i}"
        if cand not in existing:
            return cand
        i += 1


def write_rows_to_datatable(dt_path: str, new_rows: List[Dict[str, Any]], append_mode: bool, overwrite_if_exists: bool) -> None:
    dt = _load_datatable(dt_path)

    if append_mode:
        existing_rows = _read_rows_as_json_list(dt)
        existing_names = {r.get("Name") for r in existing_rows if isinstance(r, dict) and "Name" in r}

        merged = []
        if overwrite_if_exists:
            # 기존 동일 Name은 제거 후 신규로 교체
            new_names = {r.get("Name") for r in new_rows if isinstance(r, dict) and "Name" in r}
            for r in existing_rows:
                if r.get("Name") not in new_names:
                    merged.append(r)
            merged.extend(new_rows)
        else:
            # 동일 Name이면 자동 리네임
            for r in new_rows:
                nm = r.get("Name", "NewRow")
                nm2 = _make_unique_name(existing_names, nm)
                existing_names.add(nm2)
                r2 = dict(r)
                r2["Name"] = nm2
                merged.append(r2)
            merged = existing_rows + merged

        out_rows = merged
    else:
        # 테이블 전체 덮어쓰기
        out_rows = new_rows

    out_json = json.dumps(out_rows, ensure_ascii=False, indent=2)

    # UE API: fill_from_json_string(json_string, import_row_struct=None) :contentReference[oaicite:2]{index=2}
    ok = False
    if hasattr(dt, "fill_from_json_string"):
        ok = dt.fill_from_json_string(out_json, dt.get_row_struct())
    else:
        # 구버전 호환: DataTableFunctionLibrary.fill_data_table_from_json_string :contentReference[oaicite:3]{index=3}
        ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(dt, out_json)

    if not ok:
        raise RuntimeError("DataTable JSON import 실패. Output Log에서 import 에러를 확인하세요.")

    # 에셋 저장
    unreal.EditorAssetLibrary.save_asset(dt_path, only_if_is_dirty=False)
    unreal.log(f"[OK] DataTable 업데이트 완료: {dt_path} (rows={len(out_rows)})")


def main():
    row = generate_foliage_row(
        name=ROW_NAME,
        foliage_types=FOLIAGE_TYPES,
        count=COUNT,
        x_range=X_RANGE,
        y_range=Y_RANGE,
        min_dist=MIN_DIST,
        seed=SEED,
        mode=MODE,
        disk_radius=DISK_RADIUS,
        keepout_radius=KEEPOUT_RADIUS,
        z_value=Z_VALUE,
        type_weights=TYPE_WEIGHTS,
    )

    write_rows_to_datatable(
        dt_path=DATATABLE_PATH,
        new_rows=[row],
        append_mode=APPEND_MODE,
        overwrite_if_exists=OVERWRITE_IF_EXISTS,
    )


if __name__ == "__main__":
    main()
