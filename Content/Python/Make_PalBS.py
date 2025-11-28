import unreal

# ------------------------------------------------------------
# 설정
# ------------------------------------------------------------

GLOBAL_ANIM_DIR   = GamePalModelGlobalAnimation
MONSTER_ROOT      = GamePalModelMonster

TEMPLATE_ABP_NAME = ABP_Global_Template
TEMPLATE_ABP_PATH = f{GLOBAL_ANIM_DIR}{TEMPLATE_ABP_NAME}.{TEMPLATE_ABP_NAME}

GLOBAL_AS_PREFIX  = AS_MM_        # Global AnimSequence
GLOBAL_BS_PREFIX  = BS_Global_    # Global BlendSpace

PAL_AS_PREFIX     = AS_           # AS_PalName_Action
PAL_BS_PREFIX     = BS_           # BS_PalName_Action

editor_lib  = unreal.EditorAssetLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
registry    = unreal.AssetRegistryHelpers.get_asset_registry()


# ------------------------------------------------------------
# 공통 유틸
# ------------------------------------------------------------

def load_template_abp()
    abp = editor_lib.load_asset(TEMPLATE_ABP_PATH)
    if not abp
        unreal.log_error(f[오류] 템플릿 ABP 로드 실패 {TEMPLATE_ABP_PATH})
        return None
    if not isinstance(abp, unreal.AnimBlueprint)
        unreal.log_error(f[오류] 템플릿이 AnimBlueprint가 아님 {TEMPLATE_ABP_PATH})
        return None
    return abp


def find_pal_skeleton(pal_folder str)
    각 팰 폴더의 MeshSK_팰이름_Skeleton 에서 Skeleton 찾기.
    mesh_dir = pal_folder + Mesh

    if not editor_lib.does_directory_exist(mesh_dir)
        unreal.log_warning(f[SKIP] Mesh 폴더 없음 {mesh_dir})
        return None

    asset_datas = registry.get_assets_by_path(mesh_dir, recursive=True)
    for ad in asset_datas
        if ad.asset_class_path.asset_name != SkeletalMesh
            continue

        mesh = ad.get_asset()
        if not mesh
            continue

        skel = mesh.get_editor_property(skeleton)
        if skel
            return skel

    unreal.log_warning(f[SKIP] Skeleton 없음 {mesh_dir})
    return None


def collect_global_anim_assets()
    
    Global_Animation 폴더에서
      - AS_MM_ (AnimSequence)
      - BS_Global_ (BlendSpace  BlendSpace1D)
    를 수집해서 액션명 기준 dict 로 리턴.
    
    paths = editor_lib.list_assets(GLOBAL_ANIM_DIR, recursive=False)
    seq_map = {}  # action - AnimSequence
    bs_map  = {}  # action - BlendSpace

    for path in paths
        name = path.split()[-1]
        asset = editor_lib.load_asset(path)
        if not asset
            continue

        # AnimSequence
        if isinstance(asset, unreal.AnimSequence) and name.startswith(GLOBAL_AS_PREFIX)
            action = name[len(GLOBAL_AS_PREFIX)]
            seq_map[action] = asset
            continue

        # BlendSpace
        if isinstance(asset, (unreal.BlendSpace, unreal.BlendSpace1D)) and name.startswith(GLOBAL_BS_PREFIX)
            action = name[len(GLOBAL_BS_PREFIX)]
            bs_map[action] = asset
            continue

    unreal.log(f[INFO] Global AnimSequence 개수 {len(seq_map)})
    unreal.log(f[INFO] Global BlendSpace 개수 {len(bs_map)})
    return seq_map, bs_map


# ------------------------------------------------------------
# 1. 템플릿으로부터 자식 AnimBP 생성
# ------------------------------------------------------------

def create_or_load_pal_abp(template_abp unreal.AnimBlueprint,
                           pal_folder str,
                           pal_name str) - unreal.AnimBlueprint  None
    
    1. Global_Animation의 ABP_Global_Template 로부터
       각 팰용 자식 AnimBlueprint 생성 (또는 기존 거 로드).
       - Skeleton MeshSK_팰이름_Skeleton
       - 이름 ABP_팰이름
       - 위치 각 팰 폴더 루트
    1-3. 이미 있으면 생성은 스킵하고 2 단계(오버라이드)로 넘어감.
    
    pal_abp_name = fABP_{pal_name}
    pal_abp_path = f{pal_folder}{pal_abp_name}.{pal_abp_name}

    # 이미 존재하면 로드만
    if editor_lib.does_asset_exist(pal_abp_path)
        unreal.log_warning(f[SKIP] 이미 AnimBP 존재, 오버라이드만 수행 {pal_abp_path})
        pal_abp = editor_lib.load_asset(pal_abp_path)
        if not isinstance(pal_abp, unreal.AnimBlueprint)
            unreal.log_error(f[오류] 기존 에셋이 AnimBlueprint가 아님 {pal_abp_path})
            return None
        return pal_abp

    # Skeleton 찾기
    skel = find_pal_skeleton(pal_folder)
    if not skel
        return None

    # 템플릿의 GeneratedClass 를 부모로 사용
    parent_class = template_abp.generated_class()
    if not parent_class
        unreal.log_error(f[오류] 템플릿 ABP의 generated_class() 조회 실패 {TEMPLATE_ABP_PATH})
        return None

    factory = unreal.AnimBlueprintFactory()
    factory.set_editor_property(parent_class, parent_class)
    factory.set_editor_property(target_skeleton, skel)

    pal_abp = asset_tools.create_asset(
        asset_name   = pal_abp_name,
        package_path = pal_folder,   # 각 팰 폴더 루트에 배치
        asset_class  = unreal.AnimBlueprint,
        factory      = factory
    )

    if not pal_abp
        unreal.log_error(f[오류] Pal AnimBP 생성 실패 {pal_abp_name})
        return None

    unreal.log(f  [생성] {pal_abp_path})
    return pal_abp


# ------------------------------------------------------------
# 2. Asset Override 설정
# ------------------------------------------------------------

def apply_overrides_for_pal(pal_abp unreal.AnimBlueprint,
                            pal_folder str,
                            pal_name str,
                            global_seq_map dict,
                            global_bs_map dict)
    
    2. 자식 AnimBP 의 에셋 오버라이드 설정.
       - ABP_Global_Template 에 등록된 Global 애니(AS_MM_, BS_Global_) 기준
       - 각 팰의 애니로 교체
          AS_MM_Start  - AS_팰이름_Start
          BS_Global_Run - BS_팰이름_Run
       - 팰 애니가 없으면 override 를 NULL(None) 로 설정
    
    anim_lib = unreal.AnimationLibrary  # UAnimationBlueprintLibrary

    pal_anim_dir = f{pal_folder}Animation

    # AnimSequence 쪽
    for action, global_anim in global_seq_map.items()
        pal_anim_name = f{PAL_AS_PREFIX}{pal_name}_{action}
        pal_anim_path = f{pal_anim_dir}{pal_anim_name}.{pal_anim_name}

        if editor_lib.does_asset_exist(pal_anim_path)
            pal_anim = editor_lib.load_asset(pal_anim_path)
        else
            pal_anim = None

        if pal_anim
            override_asset = pal_anim
            unreal.log(f  [OVERRIDE] {global_anim.get_name()} → {pal_anim_name})
        else
            # 2-2. 애니메이션이 없으면 NULL(Override 제거)로 설정
            override_asset = None
            unreal.log_warning(f  [OVERRIDE] {global_anim.get_name()} → NULL (Pal Anim 없음 {pal_anim_path}))

        anim_lib.add_node_asset_override(
            pal_abp,
            global_anim,
            override_asset,
            False  # print_applied_overrides
        )

    # BlendSpace 쪽
    for action, global_bs in global_bs_map.items()
        pal_bs_name = f{PAL_BS_PREFIX}{pal_name}_{action}
        pal_bs_path = f{pal_anim_dir}{pal_bs_name}.{pal_bs_name}

        if editor_lib.does_asset_exist(pal_bs_path)
            pal_bs = editor_lib.load_asset(pal_bs_path)
        else
            pal_bs = None

        if pal_bs
            override_bs = pal_bs
            unreal.log(f  [OVERRIDE] {global_bs.get_name()} → {pal_bs_name})
        else
            override_bs = None
            unreal.log_warning(f  [OVERRIDE] {global_bs.get_name()} → NULL (Pal BS 없음 {pal_bs_path}))

        anim_lib.add_node_asset_override(
            pal_abp,
            global_bs,
            override_bs,
            False
        )

    # 저장
    editor_lib.save_asset(pal_abp.get_path_name())


# ------------------------------------------------------------
# main
# ------------------------------------------------------------

def main()
    unreal.log(==============================================)
    unreal.log([START] ABP_Global_Template → 각 팰 AnimBP + Override 설정)
    unreal.log(==============================================)

    template_abp = load_template_abp()
    if not template_abp
        return

    global_seq_map, global_bs_map = collect_global_anim_assets()

    pal_folders = registry.get_sub_paths(MONSTER_ROOT, recursive=False)

    for pal_folder in pal_folders
        pal_name = pal_folder.split()[-1]
        unreal.log(f------ {pal_name} 처리 시작 ------)

        # 1. 자식 AnimBP 생성  로드
        pal_abp = create_or_load_pal_abp(template_abp, pal_folder, pal_name)
        if not pal_abp
            unreal.log_warning(f[SKIP] AnimBP 생성로드 실패 {pal_name})
            continue

        # 2. 에셋 오버라이드 적용
        apply_overrides_for_pal(
            pal_abp       = pal_abp,
            pal_folder    = pal_folder,
            pal_name      = pal_name,
            global_seq_map= global_seq_map,
            global_bs_map = global_bs_map
        )

        unreal.log(f------ {pal_name} 처리 종료 ------)

    unreal.log(==============================================)
    unreal.log([완료] AnimBP 생성 및 에셋 오버라이드 전체 종료)
    unreal.log(==============================================)


if __name__ == __main__
    main()
