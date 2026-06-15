import unreal
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)


# =========================================================
# 사용자 지정 영역 (여기만 고치면 됨)
# =========================================================
# 각 팰 폴더 안에서 DataTable이 들어있는 하위 폴더 이름
# 최종 경로: {PalConfig.MONSTER_ROOT}/{pal}/{PAL_DT_SUBFOLDER}
#   = /Game/Pal/Model/Monser/{pal}/DataTable
PAL_DT_SUBFOLDER = "DataTable"

# 옮길(소스) DT 이름 규칙. {pal} 자리에 팰 이름이 들어감.
#  - 팰 이름이 들어가는 경우 :  "DT_{pal}_MonsterData"
#  - 팰별 폴더 안에서 이름이 고정인 경우 : "DT_MonsterData" ({pal} 없어도 됨)
SOURCE_DT_NAME_FMT = "DT_{pal}_MonsterData"

# 모을 대상(통합) Monster DT
DEST_DT_DIR    = "/Game/Pal/DataTable"
DEST_DT_NAME   = "DT_PalMonsterData"
DEST_DT_STRUCT = "PalMonsterData"

# True  : 소스 0번째 행을 통합 행에 머지(기존 필드 보존, 겹치는 필드만 소스값으로 덮음)
# False : 소스 0번째 행으로 통합 행을 통째로 교체
MERGE_PRESERVE = True
# =========================================================


def shift_monster_data():
    pal_names = PalConfig.get_pal_names_from_monster_root()
    if not pal_names:
        unreal.log_warning("[Shift] 팰 목록이 비어있음. MONSTER_ROOT 확인 필요.")
        return

    # 통합 DT 로드/생성 후 현재 행들을 dict 리스트로 확보
    dest_dt = PalConfig.load_or_create_datatable(DEST_DT_DIR, DEST_DT_NAME, DEST_DT_STRUCT)
    dest_rows = PalConfig.Get_DtRows_as_list(dest_dt)
    # Name -> 인덱스 (upsert용)
    index_by_name = {row.get("Name"): i for i, row in enumerate(dest_rows)}

    applied = 0
    for pal in pal_names:
        src_path = "{root}/{pal}/{sub}/{name}".format(
            root=PalConfig.MONSTER_ROOT,
            pal=pal,
            sub=PAL_DT_SUBFOLDER,
            name=SOURCE_DT_NAME_FMT.format(pal=pal),
        )

        if not unreal.EditorAssetLibrary.does_asset_exist(src_path):
            unreal.log_warning("[Shift] 소스 DT 없음, 건너뜀: {0}".format(src_path))
            continue

        src_dt = unreal.EditorAssetLibrary.load_asset(src_path)
        if not src_dt:
            unreal.log_warning("[Shift] 소스 DT 로드 실패, 건너뜀: {0}".format(src_path))
            continue

        src_rows = PalConfig.Get_DtRows_as_list(src_dt)
        if not src_rows:
            unreal.log_warning("[Shift] 소스 DT에 행이 없음(0번째 없음), 건너뜀: {0}".format(src_path))
            continue

        src_row0 = dict(src_rows[0])  # 0번째 행 복사본

        if pal in index_by_name:
            target = dest_rows[index_by_name[pal]]
            if MERGE_PRESERVE:
                merged = dict(target)   # 기존 통합 행 보존
                merged.update(src_row0)  # 겹치는 필드만 소스값으로 덮음
            else:
                merged = src_row0
            merged["Name"] = pal         # 행 키는 항상 팰 이름으로 고정
            dest_rows[index_by_name[pal]] = merged
        else:
            new_row = src_row0
            new_row["Name"] = pal
            index_by_name[pal] = len(dest_rows)
            dest_rows.append(new_row)

        applied += 1
        unreal.log("[Shift] 반영: {0}  <- {1}".format(pal, src_path))

    # 통합 DT에 일괄 적용 + 저장
    PalConfig.Apply_dt_from_rows_list(dest_dt, dest_rows)
    unreal.EditorAssetLibrary.save_loaded_asset(dest_dt)
    unreal.log("[Shift] 완료: {0}개 팰 반영 / 대상 {1}".format(applied, DEST_DT_NAME))


# 실행
shift_monster_data()
