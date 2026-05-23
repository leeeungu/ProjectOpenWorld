import unreal
import importlib
import re
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    CONFIG_PAL_NAME,
    MONSTER_ROOT,
    PAL_AS_PREFIX,
    Get_PalBp,
    find_asset,
)

# ==========================================================
# 설정 (필요 시 수정)
# ==========================================================

# AS_{PalName}_{job} 이 실존할 때만 비트/TMap 반영
PAL_JOBS = [
    "Architecture",
    "Mining",
    "Deforest",
    "Transport",
    "Cooking",
    "Planting",
    "Attack",
    "CommonWork"
]

# EPalWorkCapability (UseEnumValuesAsMaskValuesInEditor=true → value 가 곧 비트값)
WORK_CAPABILITY_BITS = {
    "Architecture": 1 << 0,   # 1
    "Mining":       1 << 1,   # 2
    "Deforest":     1 << 2,   # 4
    "Transport":    1 << 3,   # 8
    "Cooking":      1 << 4,   # 16
    "Planting":     1 << 5,   # 32
    "Attack":       1 << 6,   # 64
    "CommonWork" :  1 << 7    # 128
}

COMPONENT_NAME    = "JobComponent"
PROPERTY_NAME     = "CapabilityMask"
ABP_TMAP_PROPERTY = "WorkAnim"

PROJECT_MODULE    = "ProjectOpenWorld"
JOB_ENUM_NAME     = "PalJobType"


# ==========================================================
# 함수
# ==========================================================

def _ensure_enum_exposed() -> bool:
    """unreal.PalJobType 노출 보장 시도."""
    if hasattr(unreal, JOB_ENUM_NAME):
        return True

    if hasattr(unreal, "load_module"):
        try:
            unreal.load_module(PROJECT_MODULE)
        except Exception as e:
            unreal.log_warning(f"[ENUM] load_module 실패: {e}")

    if not hasattr(unreal, JOB_ENUM_NAME):
        try:
            unreal.load_object(None, f"/Script/{PROJECT_MODULE}.{JOB_ENUM_NAME}")
        except Exception as e:
            unreal.log_warning(f"[ENUM] load_object 실패: {e}")

    return hasattr(unreal, JOB_ENUM_NAME)


def _build_job_key_candidates(job_name: str) -> list:
    """job_name 으로 만들 수 있는 TMap key 후보들 (시도 순서대로)."""
    candidates = []

    # 1) unreal.PalJobType.<name>  (정상 노출 시)
    snake_case = re.sub(r'(?<!^)(?=[A-Z])', '_', job_name).upper()
    enum_cls = getattr(unreal, JOB_ENUM_NAME, None)
    if enum_cls is not None:
        for cand in (job_name, job_name.upper(), job_name.lower(), snake_case):
            v = getattr(enum_cls, cand, None)
             
            if v is not None:
                if v not in candidates:
                    candidates.append(v)
                unreal.log(
                    f"[candidates] {job_name}[{cand}]"
                )
    """
    # 2) load_object 로 받은 UEnum 객체의 attribute (빌드에 따라 노출됨)
    try:
    uenum = unreal.load_object(None, f"/Script/{PROJECT_MODULE}.{JOB_ENUM_NAME}")
    if uenum is not None:
        for cand in (job_name, job_name.upper(), job_name.lower()):
            v = getattr(uenum, cand, None)
            if v is not None and v not in candidates:
                candidates.append(v)
    except Exception:
        pass

    # 3) FName / str (일부 Python wrapper 가 EnumProperty 에 대해 변환 지원)
    candidates.append(unreal.Name(job_name))
    candidates.append(job_name)
    """
    return candidates


def collect_jobs_with_anim(pal_name: str) -> dict[str, unreal.AnimSequence]:
    found: dict[str, unreal.AnimSequence] = {}
    anim_dir = f"{MONSTER_ROOT}/{pal_name}/Animation"
    for job in PAL_JOBS:
        path = f"{anim_dir}/{PAL_AS_PREFIX}{pal_name}_{job}"
        if not unreal.EditorAssetLibrary.does_asset_exist(path):
            unreal.log_warning(f"[SKIP] AS_{pal_name}_{job} 없음 → 비트/TMap 제외")
            continue
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if isinstance(asset, unreal.AnimSequence):
            found[job] = asset
        else:
            unreal.log_warning(f"[SKIP] {path} 가 AnimSequence 아님")
    return found


def find_component(bp: unreal.Blueprint, component_name: str) -> unreal.Object | None:
    cdo = unreal.get_default_object(bp.generated_class())
    for comp in cdo.get_components_by_class(unreal.ActorComponent):
        if comp.get_name() == component_name:
            return comp

    subsystem = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    lib = unreal.SubobjectDataBlueprintFunctionLibrary
    for handle in subsystem.k2_gather_subobject_data_for_blueprint(context=bp):
        data = subsystem.k2_find_subobject_data_from_handle(handle)
        obj = lib.get_object(data)
        if obj is not None and obj.get_name() == component_name:
            return obj
    return None


def apply_capability_mask(pal_name: str, jobs_with_anim: dict[str, unreal.AnimSequence]) -> bool:
    bp = Get_PalBp(pal_name)
    if not bp:
        unreal.log_error(f"[오류] Pal BP 로드 실패: {pal_name}")
        return False

    comp = find_component(bp, COMPONENT_NAME)
    if not comp:
        unreal.log_error(f"[오류] '{COMPONENT_NAME}' 컴포넌트 못 찾음: {bp.get_path_name()}")
        return False

    mask = 0
    for job in jobs_with_anim:
        bit = WORK_CAPABILITY_BITS.get(job)
        if bit is None:
            unreal.log_warning(f"[WARN] EPalWorkCapability 에 '{job}' 없음 → 비트 skip")
            continue
        mask |= bit
    mask |= WORK_CAPABILITY_BITS["Attack"]
    comp.set_editor_property(PROPERTY_NAME, int(mask))
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    unreal.log(f"[BP] {bp.get_name()}.{COMPONENT_NAME}.{PROPERTY_NAME} = {mask}")
    return True


def apply_abp_job_animations(pal_name: str, jobs_with_anim: dict[str, unreal.AnimSequence]) -> bool:
    _ensure_enum_exposed()  # 가능하면 트리거. 실패해도 후보 시도는 진행.

    abp_path = f"{MONSTER_ROOT}/{pal_name}/ABP_{pal_name}_Pal"
    abp = find_asset(abp_path, unreal.AnimBlueprint)
    if not abp:
        return False

    cdo = unreal.get_default_object(abp.generated_class())
    tmap = cdo.get_editor_property(ABP_TMAP_PROPERTY)
    if tmap is None:
        unreal.log_error(f"[오류] ABP TMap 프로퍼티 '{ABP_TMAP_PROPERTY}' 못 찾음")
        return False

    written = 0
    last_errors = []
    for job, anim in jobs_with_anim.items():
        success = False
        for key in _build_job_key_candidates(job):
            try:
                tmap[key] = anim
                written += 1
                success = True
                unreal.log(
                    f"[ABP] {ABP_TMAP_PROPERTY}[{job}] = {anim.get_name()} "
                    f"(key 타입: {type(key).__name__})"
                )
                break
            except Exception as e:
                last_errors.append(f"{job} / {type(key).__name__} → {e}")
                continue
        if not success:
            unreal.log_warning(f"[WARN] '{job}' 모든 key 형식 실패")

    if written == 0:
        unreal.log_error(
            "[오류] TMap 에 하나도 못 넣음.\n"
            "  원인 가능성:\n"
            "    1) unreal.PalJobType 이 Python 에 노출 안 됨 "
            "→ ★ 에디터 재시작 후 재실행 ★ (C++ 모듈은 빌드/리로드 됐어도 Python wrapper 는 갱신 안 될 수 있음)\n"
            "    2) ABP_TMAP_PROPERTY 의 value 타입이 TSoftObjectPtr<UAnimSequence> → SoftObjectPath 로 감싸야 함"
        )
        return False
    for err in last_errors[:6]:
        unreal.log_error(f"  · {err}")
        

    cdo.set_editor_property(ABP_TMAP_PROPERTY, tmap)
    unreal.BlueprintEditorLibrary.compile_blueprint(abp)
    unreal.EditorAssetLibrary.save_loaded_asset(abp)
    return True

def get_pal_names_from_monster_root() -> list[str]:
    """
    MONSTER_ROOT 하위의 1차 폴더명을 전부 Pal 이름으로 간주해서 반환.
    예: /Game/Pal/Model/Monster/Anubis, /Game/Pal/Model/Monster/Fox ...
    """
    editor_lib = unreal.EditorAssetLibrary
    root = MONSTER_ROOT  # "/Game/Pal/Model/Monster"

    # 모든 에셋/폴더 경로를 가져온 뒤, MONSTER_ROOT 기준 첫 세그먼트만 추출
    all_paths = editor_lib.list_assets(root, True, True)
    pal_names = set()

    for path in all_paths:
        # path 예: "/Game/Pal/Model/Monster/Anubis", "/Game/Pal/Model/Monster/Anubis/Animation/AS_..."
        rel = str(path)[len(root):].lstrip("/")
        if not rel:
            continue

        first = rel.split("/")[0]
        if not first:
            continue

        pal_folder = f"{root}/{first}"
        if editor_lib.does_directory_exist(pal_folder):
            pal_names.add(first)

    return sorted(pal_names)

# ==========================================================
# main
# ==========================================================

def main():
    pal_names = get_pal_names_from_monster_root()
    for pal_name in pal_names:
        unreal.log(f"[Python] Start : 펠 네임 {pal_name} Capability + ABP JobAnim 설정")
        jobs_with_anim = collect_jobs_with_anim(pal_name)
        if not jobs_with_anim:
            unreal.log_warning("[종료] 적용할 직업 애니가 하나도 없음")
            continue
        apply_capability_mask(pal_name, jobs_with_anim)
        apply_abp_job_animations(pal_name, jobs_with_anim)
        unreal.log(f"[Python] End : 펠 네임 {pal_name} Capability + ABP JobAnim 설정 완료")


if __name__ == "__main__":
    main()
