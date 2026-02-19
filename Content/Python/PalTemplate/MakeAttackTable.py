import unreal
import json
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    MONSTER_ROOT,
    PAL_BP_PREFIX,
    CONFIG_PAL_NAME,
    PAL_ATTACK_DT_STRUCT_PATH,
    ANIM_SEQUENCE_NAMES,
)


# -----------------------------
# 경로/에셋 유틸
# -----------------------------
def ensure_directory(dir_path: str):
    if not unreal.EditorAssetLibrary.does_directory_exist(dir_path):
        unreal.EditorAssetLibrary.make_directory(dir_path)

def list_assets_of_class(folder: str, cls, recursive: bool = True):
    if not unreal.EditorAssetLibrary.does_directory_exist(folder):
        return []

    prefix = f"AS_{CONFIG_PAL_NAME}_"

    # 1) 원하는 이름 목록을 '순서 유지'한 채로 full name으로 정규화
    wanted_names_in_order = []
    for n in ANIM_SEQUENCE_NAMES:
        
        if not isinstance(n, str) or not n:
            continue
        full = n if n.startswith(prefix) else f"{prefix}{n}"
        wanted_names_in_order.append(full)
        
    
    asset_paths = unreal.EditorAssetLibrary.list_assets(folder, recursive=recursive, include_folder=False)


    out = []
    missing = []
    
    for asset_name in wanted_names_in_order:
        p = asset_name
        if not p:
            missing.append(asset_name)
            continue

        obj = unreal.EditorAssetLibrary.load_asset(f"{folder}/{p}")
        if isinstance(obj, cls):
            out.append(obj)
        else:
            unreal.log_warning(f"[Anim] 타입 불일치: {asset_name} (기대={cls.__name__})")

    if missing:
        unreal.log_warning(f"[Anim] 폴더에서 못 찾은 AnimSequence: {missing}")
    return out

def get_object_path(asset_obj) -> str:
    # "/Game/.../Asset.Asset" 형태
    return unreal.EditorAssetLibrary.get_path_name_for_loaded_asset(asset_obj)

def does_asset_exist(asset_path_no_dot: str) -> bool:
    # "/Game/.../AssetName" 경로 기준으로 체크
    return unreal.EditorAssetLibrary.does_asset_exist(asset_path_no_dot)

def load_asset(asset_path_no_dot: str):
    return unreal.EditorAssetLibrary.load_asset(asset_path_no_dot)

def save_asset(asset_obj):
    unreal.EditorAssetLibrary.save_loaded_asset(asset_obj)

def resolve_row_struct_fpal_attackdatatable():
    """
    FPalAttackDataTable의 리플렉션 이름은 보통 'PalAttackDataTable'(F 제거)입니다.
    모듈명은 프로젝트명(=SystemLibrary.get_project_name())으로 우선 추정합니다.
    """
    struct_name = "PalAttackDataTable"  # FPalAttackDataTable -> PalAttackDataTable
    ##project_module = unreal.SystemLibrary.get_project_name()

    # 1) 가장 흔한 형태: /Script/<ProjectModule>.<StructName>
    candidates = [
        PAL_ATTACK_DT_STRUCT_PATH,
        struct_name,  # find_object fallback 용
    ]

    for c in candidates:
        try:
            obj = unreal.load_object(None, c)
            if obj:
                return obj
        except Exception:
            pass

        try:
            obj = unreal.find_object(None, c)
            if obj:
                return obj
        except Exception:
            pass

    raise RuntimeError(
        f"RowStruct 'FPalAttackDataTable'을 찾지 못했습니다. "
        f"resolve_row_struct_fpal_attackdatatable()에 실제 스크립트 경로를 추가하세요. "
        f"(예: /Script/ProjectOpenWorld.PalAttackDataTable)"
    )


# -----------------------------
# Montage 생성
# -----------------------------
def create_or_load_montage_from_sequence(seq: unreal.AnimSequence, dest_folder: str) -> unreal.AnimMontage | None:
    seq_name = seq.get_name()
    if seq_name.startswith("AS_"):
        montage_name = "AM_" + seq_name[3:]
    else:
        montage_name = f"AM_{seq_name}"
    montage_asset_path = f"{dest_folder}/{montage_name}"

    # 이미 있으면 로드
    if does_asset_exist(montage_asset_path):
        existing = load_asset(montage_asset_path.split('.')[0])
        if isinstance(existing, unreal.AnimMontage):
            return existing
    try:
        dirty = False

        desired_enable = True
        desired_root_lock = unreal.RootMotionRootLock.ANIM_FIRST_FRAME  # 필요 시 변경
        desired_force_lock = False
        desired_norm_scale = True

        if seq.get_editor_property("enable_root_motion") != desired_enable:
            seq.set_editor_property("enable_root_motion", desired_enable)
            dirty = True



        if dirty:
            unreal.EditorAssetLibrary.save_loaded_asset(seq)
    except Exception as e:
        unreal.log_warning(f"[RootMotion] 설정 실패: {seq.get_path_name()} / {e}")

    factory = unreal.AnimMontageFactory()
    factory.set_editor_property("source_animation", seq)
    factory.set_editor_property("target_skeleton", seq.get_editor_property("skeleton"))

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    montage = asset_tools.create_asset(montage_name, dest_folder, unreal.AnimMontage, factory)
    #MonName_list = list(seq_name)
    #MonName_list[1] = 'M'
    #MonName = "".join(MonName_list)
    #unreal.log_warning(unreal.Name(MonName))
    #montage.rename(unreal.Name(MonName))
    if montage:
       save_asset(montage)
    else:
       unreal.warning(f"[Montage] : {montage_name} Montage Create 실패")
    return montage


# -----------------------------
# DataTable JSON 패치
# -----------------------------
def _export_dt_rows_as_list(dt: unreal.DataTable) -> list[dict]:
    s = dt.export_to_json_string()
    if not s:
        return []
    data = json.loads(s)

    # UE는 보통 list 포맷으로 내보냅니다. 혹시 dict면 list로 normalize.
    if isinstance(data, list):
        return data
    if isinstance(data, dict):
        rows = []
        for k, v in data.items():
            if isinstance(v, dict):
                r = dict(v)
                r.setdefault("Name", k)
                rows.append(r)
        return rows

    return []

def _fill_dt_from_rows_list(dt: unreal.DataTable, rows: list[dict]) -> bool:
    # enum(AttackSlot) 표기 호환을 위해 두 번 시도: "Default" -> 1
    row_struct = dt.get_row_struct()
    json_str = json.dumps(rows, ensure_ascii=False, indent=2)

    ok = dt.fill_from_json_string(json_str, row_struct)
    if ok:
        return True

    # 실패 시: AttackSlot이 문자열 enum을 못 받아들이는 케이스 대비 → 숫자(1)로 강제
    for r in rows:
        if "AttackSlot" in r and isinstance(r["AttackSlot"], str):
            r["AttackSlot"] = 1
    json_str2 = json.dumps(rows, ensure_ascii=False, indent=2)
    ok2 = dt.fill_from_json_string(json_str2, row_struct)
    return bool(ok2)

def load_or_create_datatable(dt_folder: str, dt_name: str) -> unreal.DataTable:
    ensure_directory(dt_folder)

    dt_asset_path = f"{dt_folder}/{dt_name}"
    if does_asset_exist(dt_asset_path):
        dt = load_asset(dt_asset_path)
        if not isinstance(dt, unreal.DataTable):
            raise RuntimeError(f"경로에 DataTable이 아닌 에셋이 존재합니다: {dt_asset_path}")
        return dt

    # 없으면 생성
    row_struct = resolve_row_struct_fpal_attackdatatable()
    factory = unreal.DataTableFactory()
    factory.set_editor_property("struct", row_struct)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    dt = asset_tools.create_asset(dt_name, dt_folder, unreal.DataTable, factory)
    if not dt:
        raise RuntimeError(f"DataTable 생성 실패: {dt_asset_path}")
    save_asset(dt)
    return dt

def make_soft_object_entry(object_path: str) -> dict:
    # UPROPERTY가 TSoftObjectPtr<UAnimMontage> 또는 FSoftObjectPath 류일 때 통하는 포맷
    return {"AssetPathName": object_path, "SubPathString": ""}

def set_second_row_attackdata_if_empty(dt: unreal.DataTable, montage_object_paths: list[str]) -> bool:
    """
    규칙:
    - row가 2개 미만이면 Row0/Row1 생성
    - 2번째 row의 AttackSlot = Default(문자열) 시도, 안되면 1로 fallback
    - 2번째 row의 AttackData가 이미 비어있지 않으면 절대 수정/추가 안 함
    """
    rows = _export_dt_rows_as_list(dt)

    # row 0/1 보장 (이름은 Row0/Row1로 신규 생성)
    if len(rows) == 0:
        rows.append({"Name": "Row0"})
        rows.append({"Name": "Row1"})
    elif len(rows) == 1:
        # 기존 1개를 Row0로 취급, Row1 생성
        rows.append({"Name": "Row1"})

    # 2번째 row
    row2 = rows[1]

    # AttackData 이미 있으면 스킵
    current_attackdata = row2.get("AttackData", [])
    if isinstance(current_attackdata, list) and len(current_attackdata) > 0:
        unreal.log_warning("[DT] 2번째 Row의 AttackData에 이미 데이터가 존재합니다. 요구사항에 따라 수정/추가하지 않습니다.")
        return False

    # AttackSlot 설정 (문자열 Default 우선, 실패 시 fill 단계에서 1로 fallback)
    row2["AttackSlot"] = "Default"  # ESubAttackType::Default 기대

    # AttackData 포맷: SoftObjectPath dict (일반적으로 TSoftObjectPtr/SoftObjectPath에 적합)
   
    row2["AttackData"] = montage_object_paths
    
    ok = _fill_dt_from_rows_list(dt, rows)
    if not ok:
        raise RuntimeError("[DT] fill_from_json_string 실패. AttackData 요소 타입/구조(배열 원소 타입)가 현재 포맷과 다를 가능성이 큽니다.")

    save_asset(dt)
    unreal.log(f"[DT] 2번째 Row 업데이트 완료: AttackData={len(montage_object_paths)}개")
    return True


# -----------------------------
# 메인
# -----------------------------
def run_build_attack_montages_and_patch_dt():
    unreal.log(CONFIG_PAL_NAME)
    pal_root = f"{MONSTER_ROOT}/{CONFIG_PAL_NAME}"
    anim_folder = f"{pal_root}/Animation"
    montage_folder = f"{pal_root}/Montage"
    dt_folder = f"{pal_root}/DataTable"
    dt_name = f"DT_{CONFIG_PAL_NAME}_AttackData"

    # 폴더 보장
    ensure_directory(montage_folder)
    ensure_directory(dt_folder)

    # AnimSequence 수집
    sequences = list_assets_of_class(anim_folder, unreal.AnimSequence, recursive=True)
    unreal.log(f"[Seq] : {sequences} / [Folder] :{anim_folder}")
    if not sequences:
        unreal.log_warning(f"[Anim] AnimSequence가 없습니다: {anim_folder}")
        return

    # Montage 생성/로드
    montages = []
    for seq in sequences:
        m = create_or_load_montage_from_sequence(seq, montage_folder)
        if m:
            montages.append(m)

    if not montages:
        unreal.log_warning("[Montage] 생성/로드된 AnimMontage가 없습니다.")
        return

    montage_paths = [get_object_path(m) for m in montages]

    # DataTable 로드/생성
    dt = load_or_create_datatable(dt_folder, dt_name)

    # 트랜잭션(Undo 지원)
    with unreal.ScopedEditorTransaction("Build Pal Attack Montages + Patch AttackData(Second Row)"):
        set_second_row_attackdata_if_empty(dt, montage_paths)


def set_pal_attack_component_attack_data_asset_to_dt(TEMPLATE_NAME : str):
    """
    전제: 아래 전역은 외부 모듈에서 import되어 있음
      - MONSTER_ROOT
      - CONFIG_PAL_NAME
      - PAL_BP_PREFIX
      - TEMPLATE_NAME
    결과:
      - BP의 UPalAttackComponent.AttackDataAsset = DT_{CONFIG_PAL_NAME}_AttackData 로 설정
    """
    unreal.log(f"[Python] Start : 펠 네임 {CONFIG_PAL_NAME} AttackDataTable")
    # BP / DT 경로 구성
    bp_name = f"{PAL_BP_PREFIX}{CONFIG_PAL_NAME}_{TEMPLATE_NAME}"
    bp_path = f"{MONSTER_ROOT}/{CONFIG_PAL_NAME}/{bp_name}"

    dt_name = f"DT_{CONFIG_PAL_NAME}_AttackData"
    dt_path = f"{MONSTER_ROOT}/{CONFIG_PAL_NAME}/DataTable/{dt_name}"

    # 로드
    if not unreal.EditorAssetLibrary.does_asset_exist(bp_path):
        unreal.log_error(f"[BP] 에셋 없음: {bp_path}")
        return False
    if not unreal.EditorAssetLibrary.does_asset_exist(dt_path):
        unreal.log_error(f"[DT] 에셋 없음: {dt_path}")
        return False

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    dt = unreal.EditorAssetLibrary.load_asset(dt_path)

    if not bp:
        unreal.log_error(f"[BP] 로드 실패: {bp_path}")
        return False
    if not dt or not isinstance(dt, unreal.DataTable):
        unreal.log_error(f"[DT] 로드 실패 또는 DataTable 아님: {dt_path}")
        return False

    with unreal.ScopedEditorTransaction("Set AttackComponent.AttackDataAsset"):
        attack_comp = None

        # 1) CDO의 UPROPERTY(AttackComponent)로 직접 접근 (가장 정확)
        gen_cls = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        if gen_cls:
            cdo = unreal.get_default_object(gen_cls)  
            if cdo:
                try:
                    attack_comp = cdo.get_editor_property("AttackComponent")
                except Exception as e:
                    unreal.log_warning(f"[BP] CDO에서 AttackComponent 프로퍼티 접근 실패: {e}")

        # 2) (Fallback) SCS에서 variable_name == AttackComponent 인 노드 찾기
        if not attack_comp:
            try:
                scs = bp.get_editor_property("simple_construction_script")
                if scs:
                    for node in scs.get_all_nodes():
                        try:
                            var_name = node.get_editor_property("variable_name")  # FName
                            if str(var_name) == "AttackComponent":
                                attack_comp = node.get_editor_property("component_template")
                                break
                        except Exception:
                            pass
            except Exception:
                pass

        if not attack_comp:
            # 디버깅용으로 CDO의 컴포넌트 목록을 찍어준다
            try:
                if gen_cls:
                    cdo = gen_cls.get_default_object()
                    comps = cdo.get_components_by_class(unreal.ActorComponent) if cdo else []
                    debug = [f"{c.get_name()} ({c.get_class().get_name()})" for c in comps]
                    unreal.log_warning("[BP] CDO Components: " + ", ".join(debug))
            except Exception:
                pass

            unreal.log_error("[BP] AttackComponent 포인터를 찾지 못했습니다. (CDO 프로퍼티가 None이거나 BP에서 미할당)")
            return False

        # 3) AttackDataAsset 세팅
        try:
            attack_comp.set_editor_property("AttackDataAsset", dt)
        except Exception as e:
            unreal.log_error(
                f"[BP] AttackComponent에 AttackDataAsset 세팅 실패: {e}\n"
                f" - AttackComponent 클래스: {attack_comp.get_class().get_name()}\n"
                f" - AttackDataAsset 프로퍼티명이 실제로 다른지 확인 필요"
            )
            return False

        # 컴파일 + 저장
        try:
            unreal.KismetCompilerLibrary.compile_blueprint(bp)
        except Exception:
            pass

        unreal.EditorAssetLibrary.save_loaded_asset(bp)
    unreal.log(f"[Python] End : 펠 네임 {CONFIG_PAL_NAME} AttackDataTable 생성 성공")
    return True

# 실행
#run_build_attack_montages_and_patch_dt()
