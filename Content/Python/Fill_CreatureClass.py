# -*- coding: utf-8 -*-
#
# Fill_CreatureClass.py
#  - DT_SpawnCharacterData 의 각 row(=pal 이름)에 대해
#    명명 규칙 Bp_{Name}_Creature 로 길들여진 creature 클래스를 찾아
#    FSpawnCharacterData.CreatureClass 칸에 자동 주입한다.
#  - 구조체에 아래 칸을 먼저 추가해 두어야 한다.
#      UPROPERTY(EditAnywhere, BlueprintReadOnly)
#      TSubclassOf<APalBaseCreature> CreatureClass = nullptr;
#
# 동작 방식: export(JSON) -> CreatureClass 만 수정 -> fill(JSON) 라운드트립.
#   (fill 계열은 "비우고 새로 채우기" 라서 다른 필드 보존을 위해 반드시 export 값을 통째로 되돌린다)
#
# 검증된 API (UE 5.4 unreal.DataTableFunctionLibrary):
#   export_data_table_to_json_string / fill_data_table_from_json_string /
#   get_data_table_row_struct / export_data_table_to_json_file

import unreal
import json

# ====== 설정 ======
DT_PATH               = "/Game/Pal/DataTable/DT_PalMonsterData.DT_PalMonsterData"
MONSTER_ROOT          = "/Game/Pal/Model/Monster"
CREATURE_BP_FORMAT    = "Bp_{name}_Pal"   # 에셋 이름 규칙
CREATURE_CLASS_COLUMN = "CreatureClass"        # 구조체에 추가한 UPROPERTY 이름 (= JSON 키)
BACKUP_JSON_PATH      = "C:/Temp/DT_SpawnCharacterData_backup.json"  # 실행 전 백업 위치

DRY_RUN    = False   # True: 매칭 결과만 로그. 확인 후 False 로 바꿔 실제 기록.
ONLY_NAMES = []     # 비우면 전체. ["Anubis"] 처럼 넣으면 해당 row 만 갱신(검증용).

editor_lib = unreal.EditorAssetLibrary
dt_lib     = unreal.DataTableFunctionLibrary


def resolve_creature_class_path(name: str):
    """name 으로 Bp_{name}_Creature 의 generated class 경로를 반환. 없으면 None."""
    bp_name    = CREATURE_BP_FORMAT.format(name=name)
    asset_path = f"{MONSTER_ROOT}/{name}/{bp_name}"

    if not editor_lib.does_asset_exist(asset_path):
        unreal.log_warning(f"[SKIP] creature BP 없음: {asset_path}")
        return None

    gen_cls = editor_lib.load_blueprint_class(asset_path)
    if not gen_cls:
        unreal.log_warning(f"[SKIP] generated class 로드 실패: {asset_path}")
        return None

    # round-trip 형식과 일치시키려 실제 path_name(.._C) 을 그대로 사용
    return gen_cls.get_path_name()


def main():
    dt = editor_lib.load_asset(DT_PATH)
    if not dt or not isinstance(dt, unreal.DataTable):
        unreal.log_error(f"[오류] DataTable 로드 실패: {DT_PATH}")
        return

    # 1) 현재 상태 export (다른 필드 보존 + 백업)
    json_str = dt_lib.export_data_table_to_json_string(dt)
    if not json_str:
        unreal.log_error("[오류] DataTable JSON export 실패 (Output Log 확인)")
        return

    if not DRY_RUN:
        if dt_lib.export_data_table_to_json_file(dt, BACKUP_JSON_PATH):
            unreal.log(f"[백업] {BACKUP_JSON_PATH}")
        else:
            unreal.log_warning(f"[백업 실패] 경로 확인: {BACKUP_JSON_PATH} (계속 진행)")

    try:
        rows = json.loads(json_str)
    except json.JSONDecodeError as e:
        unreal.log_error(f"[오류] export JSON 파싱 실패: {e}")
        return
    if not isinstance(rows, list) or not rows:
        unreal.log_error("[오류] 예상치 못한 JSON 구조 (비어있거나 list 아님)")
        return

    # 칼럼명(CreatureClass) 이 맞는지 직접 검증할 수 있게 첫 row 키 출력
    unreal.log(f"[검증] 첫 row 키 목록: {sorted(rows[0].keys())}")
    if CREATURE_CLASS_COLUMN not in rows[0]:
        unreal.log_warning(
            f"[주의] '{CREATURE_CLASS_COLUMN}' 키가 export 에 없음. "
            f"구조체에 칸을 추가/컴파일했는지, 칼럼명이 맞는지 확인. (없으면 새 키로 추가됨)"
        )

    only = set(ONLY_NAMES)
    updated = skipped = 0
    for row in rows:
        row_name = str(row.get("Name", ""))
        if not row_name:
            unreal.log_warning(f"[SKIP] Name 없는 row: {row}")
            skipped += 1
            continue
        if only and row_name not in only:
            continue

        class_path = resolve_creature_class_path(row_name)
        if not class_path:
            skipped += 1
            continue

        row[CREATURE_CLASS_COLUMN] = class_path
        unreal.log(f"[MATCH] {row_name} -> {class_path}")
        updated += 1

    unreal.log(f"[요약] 갱신 {updated} / 스킵 {skipped} / 전체 {len(rows)}")

    if DRY_RUN:
        unreal.log("[DRY_RUN] 실제 기록 안 함. 결과 확인 후 DRY_RUN=False 로 다시 실행.")
        return
    if updated == 0:
        unreal.log_warning("[중단] 갱신 대상 0건. fill 생략.")
        return

    # 2) 수정본으로 되채움 (row_struct 지정 시 대화상자 없이 자동 import)
    row_struct = dt_lib.get_data_table_row_struct(dt)
    new_json   = json.dumps(rows, ensure_ascii=False)
    if not dt_lib.fill_data_table_from_json_string(dt, new_json, row_struct):
        unreal.log_error("[오류] fill_data_table_from_json_string 실패. 백업으로 복구 가능.")
        return

    # 3) 저장
    editor_lib.save_asset(DT_PATH, only_if_is_dirty=False)
    unreal.log(f"[완료] DataTable 갱신/저장: {DT_PATH} (갱신 {updated} rows)")


if __name__ == "__main__":
    main()
