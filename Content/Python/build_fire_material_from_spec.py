# -*- coding: utf-8 -*-
"""
FModel/CUE4Parse 머티리얼 덤프(JSON)에서 "파라미터 인터페이스"만 추출하고,
그 이름/타입/기본값에 담긴 의도를 해석해 *새로운* 불꽃 머티리얼 그래프를 생성·연결한다.
(UE 5.4 Editor Python)

핵심:
  - 원본 expression 그래프는 덤프에 없으므로 복제가 아니라 "재설계"다.
  - CachedExpressionData의 ParameterInfoSet ↔ ScalarValues/VectorValues/TextureValues는
    인덱스로 1:1 대응 → 여기서 스펙을 뽑는다. (extract_spec)
  - 빌트인 노드의 핀 이름은 공식 Python 문서에 안 나와서 일부는 추정. 그래서 _link/_set은
    실패 시 크래시 대신 경고 로그만 남기고 속성 fallback에 의존하도록 했다.

사용:
  import build_fire_material_from_spec as b
  spec = b.extract_spec_from_file("D:/data/M_Pal_Fire.json")
  b.print_spec(spec)
  b.build_fire_material(spec, "/Game/ProjectOpenWorld/Material/FX", "M_Fire_Reauthored")
"""

import json
import unreal
import os

MEL = unreal.MaterialEditingLibrary
EAL = unreal.EditorAssetLibrary
AssetTools = unreal.AssetToolsHelpers.get_asset_tools()


# ===========================================================================
# 1) 스펙 추출 (덤프 → 파라미터 인터페이스)  — 전부 검증된 동작
# ===========================================================================

def extract_spec(data):
    """덤프(list 또는 dict) → {name, properties, scalars, vectors, textures}."""
    obj = data[0] if isinstance(data, list) else data
    ced = obj.get("CachedExpressionData", {})

    # "RuntimeEntries", "RuntimeEntries[1]", ... 를 인덱스 순으로 정렬.
    # 버킷 인덱스는 EMaterialParameterType 순서: 0=Scalar, 1=Vector, 3=Texture.
    entry_keys = sorted(
        [k for k in ced if k.startswith("RuntimeEntries")],
        key=lambda k: int(k[len("RuntimeEntries"):].strip("[]") or "0"),
    )

    def names(i):
        if i < len(entry_keys):
            return [p["Name"] for p in ced[entry_keys[i]].get("ParameterInfoSet", [])]
        return []

    scalars = dict(zip(names(0), ced.get("ScalarValues", [])))
    vectors = {
        nm: (c["R"], c["G"], c["B"], c["A"])
        for nm, c in zip(names(1), ced.get("VectorValues", []))
    }
    textures = {
        nm: t.get("AssetPathName", "")
        for nm, t in zip(names(3), ced.get("TextureValues", []))
    }
    return {
        "name": obj.get("Name"),
        "properties": obj.get("Properties", {}),
        "scalars": scalars,
        "vectors": vectors,
        "textures": textures,
    }

def _resolve(path):
    """절대 디스크 경로면 그대로, '/Game/..' 또는 'Game\\..' 콘텐츠 경로면 디스크 경로로 변환."""
    p = path.replace("\\", "/")
    if os.path.isabs(p) and not p.startswith("/Game"):
        return p
    rel = p.split("/Game/", 1)[-1] if "/Game/" in p else p.lstrip("/")
    if rel.startswith("Game/"):
        rel = rel[len("Game/"):]
    return os.path.join(unreal.Paths.project_content_dir(), rel)

def extract_spec_from_file(json_path):
    with open(_resolve(json_path), "r", encoding="utf-8") as f:
        return extract_spec(json.load(f))

def print_spec(spec):
    unreal.log("=== Spec: %s ===" % spec["name"])
    unreal.log("[Scalars] %d" % len(spec["scalars"]))
    for k, v in spec["scalars"].items():
        unreal.log("  %-32s = %s" % (k, v))
    unreal.log("[Vectors] %d" % len(spec["vectors"]))
    for k, v in spec["vectors"].items():
        unreal.log("  %-32s = %s" % (k, v))
    unreal.log("[Textures] %d" % len(spec["textures"]))
    for k, v in spec["textures"].items():
        unreal.log("  %-32s = %s" % (k, v))


# ===========================================================================
# 2) 그래프 빌드용 안전 헬퍼
# ===========================================================================

def _set(node, prop, value):
    """set_editor_property 실패해도 죽지 않게 감싼다(핀/속성명 추정 부분 대비)."""
    try:
        node.set_editor_property(prop, value)
    except Exception as e:
        unreal.log_warning("[set] %s.%s 실패: %s" % (node.get_name(), prop, e))


def _link(src, dst, dst_input="", src_output=""):
    """expression -> expression 연결. dst_input/src_output 비우면 '첫 핀' 사용."""
    if not MEL.connect_material_expressions(src, src_output, dst, dst_input):
        unreal.log_warning("[link] %s(%s) -> %s(%s) 실패"
                           % (src.get_name(), src_output or "<first>",
                              dst.get_name(), dst_input or "<first>"))


def _to_prop(src, prop_enum, src_output=""):
    if not MEL.connect_material_property(src, src_output, prop_enum):
        unreal.log_warning("[prop] %s -> %s 실패" % (src.get_name(), prop_enum))


# ===========================================================================
# 3) 불꽃 머티리얼 재설계 빌드
# ===========================================================================

def build_fire_material(spec, package_path, name, overwrite=True):
    package_path = package_path.rstrip("/")
    full = "%s/%s" % (package_path, name)
    if EAL.does_asset_exist(full):
        if not overwrite:
            unreal.log_warning("이미 존재, 건너뜀: %s" % full)
            return None
        EAL.delete_asset(full)

    mat = AssetTools.create_asset(name, package_path, unreal.Material,
                                  unreal.MaterialFactoryNew())
    if mat is None:
        unreal.log_error("생성 실패: %s" % full)
        return None

    # --- 머티리얼 설정 (덤프 Properties 기반) ---
    _set(mat, "blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)   # 반투명 FX
    _set(mat, "shading_model", unreal.MaterialShadingModel.MSM_UNLIT)  # 자체 발광
    _set(mat, "two_sided", True)
    try:
        MEL.set_material_usage(mat, unreal.MaterialUsage.MATERIALUSAGE_SKELETAL_MESH)
    except Exception as e:
        unreal.log_warning("[usage] SkeletalMesh 설정 실패: %s" % e)

    s = spec["scalars"]
    v = spec["vectors"]

    # --- 파라미터 노드 팩토리 ---
    def scalar(pname, x, y, group="Surface"):
        n = MEL.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, x, y)
        _set(n, "parameter_name", pname)
        _set(n, "default_value", float(s.get(pname, 0.0)))
        _set(n, "group", group)
        return n

    def vector(pname, x, y, group="Color"):
        n = MEL.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, x, y)
        _set(n, "parameter_name", pname)
        c = v.get(pname, (0.0, 0.0, 0.0, 1.0))
        _set(n, "default_value", unreal.LinearColor(c[0], c[1], c[2], c[3]))
        _set(n, "group", group)
        return n

    def texparam(pname, x, y):
        # 덤프의 텍스처 경로는 원본(Pal) 프로젝트 것이라 의도적으로 비워둔다.
        # 사용자가 자신의 불꽃/노이즈 텍스처를 인스턴스에서 할당.
        n = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, x, y)
        _set(n, "parameter_name", pname)
        return n

    def make(cls, x, y):
        return MEL.create_material_expression(mat, cls, x, y)

    # ---------------------------------------------------------------
    # (A) UV 애니메이션: TexCoord -> Panner(speed = Append(USpeed,VSpeed))
    #     불꽃이 위로 흐르는 메인 모션. Panner.Time 미연결 = 머티리얼 글로벌 Time 사용.
    # ---------------------------------------------------------------
    main_tc = make(unreal.MaterialExpressionTextureCoordinate, -1400, -200)
    main_speed = make(unreal.MaterialExpressionAppendVector, -1400, 0)
    _link(scalar("MainUSpeed", -1700, -40, "Main UV"), main_speed, "A")
    _link(scalar("MainVSpeed", -1700, 60, "Main UV"), main_speed, "B")
    main_panner = make(unreal.MaterialExpressionPanner, -1150, -150)
    _set(main_panner, "speed_x", float(s.get("MainUSpeed", 0.0)))  # 핀 실패 대비 fallback
    _set(main_panner, "speed_y", float(s.get("MainVSpeed", 0.0)))
    _link(main_tc, main_panner, "Coordinate")
    _link(main_speed, main_panner, "Speed")

    noise_tc = make(unreal.MaterialExpressionTextureCoordinate, -1400, 320)
    noise_speed = make(unreal.MaterialExpressionAppendVector, -1400, 520)
    _link(scalar("NoiseUSpeed", -1700, 480, "Noise UV"), noise_speed, "A")
    _link(scalar("NoiseVSpeed", -1700, 580, "Noise UV"), noise_speed, "B")
    noise_panner = make(unreal.MaterialExpressionPanner, -1150, 360)
    _set(noise_panner, "speed_x", float(s.get("NoiseUSpeed", 0.0)))
    _set(noise_panner, "speed_y", float(s.get("NoiseVSpeed", 0.5)))
    _link(noise_tc, noise_panner, "Coordinate")
    _link(noise_speed, noise_panner, "Speed")

    # ---------------------------------------------------------------
    # (B) 디스토션: NoiseTex 샘플 * DistortionScale 를 메인 UV에 더해 일렁임.
    # ---------------------------------------------------------------
    noise_tex = texparam("NoiseTex", -900, 360)
    _link(noise_panner, noise_tex, "UVs")
    distort_mul = make(unreal.MaterialExpressionMultiply, -650, 360)
    _link(noise_tex, distort_mul, "A", src_output="R")
    _link(scalar("DistortionScale", -900, 600, "Noise UV"), distort_mul, "B")
    distorted_uv = make(unreal.MaterialExpressionAdd, -650, 80)
    _link(main_panner, distorted_uv, "A")
    _link(distort_mul, distorted_uv, "B")  # 스칼라 broadcast → 2축 동일 오프셋(워블)

    # ---------------------------------------------------------------
    # (C) 메인 샘플 + 3색 컬러 램프(Color3→Color2→Color1).
    #     텍스처 밝기(t)가 낮으면 base(red), 높으면 hot(yellow). HDR(R>1)=발광 강도.
    # ---------------------------------------------------------------
    main_tex = texparam("MainTex", -400, -150)
    _link(distorted_uv, main_tex, "UVs")

    lerp_lo = make(unreal.MaterialExpressionLinearInterpolate, -100, -200)
    _link(vector("Color3", -400, 250), lerp_lo, "A")   # base
    _link(vector("Color2", -400, 380), lerp_lo, "B")   # mid
    _link(main_tex, lerp_lo, "Alpha", src_output="R")

    lerp_hi = make(unreal.MaterialExpressionLinearInterpolate, 150, -150)
    _link(lerp_lo, lerp_hi, "A")
    _link(vector("Color1", -400, 510), lerp_hi, "B")   # hot tip
    _link(main_tex, lerp_hi, "Alpha", src_output="R")

    # ---------------------------------------------------------------
    # (D) 프레넬 림: 가장자리 글로우. exponent = rgbFresnelPower.
    # ---------------------------------------------------------------
    fresnel = make(unreal.MaterialExpressionFresnel, -100, 250)
    _set(fresnel, "exponent", float(s.get("rgbFresnelPower", 5.0)))  # 속성 fallback
    _link(scalar("rgbFresnelPower", -400, 650, "Fresnel"), fresnel, "ExponentIn")
    rim_mul = make(unreal.MaterialExpressionMultiply, 150, 250)
    _link(fresnel, rim_mul, "A")
    _link(vector("FresnelColor", -400, 640), rim_mul, "B")

    # Emissive = 램프 + 림
    emissive = make(unreal.MaterialExpressionAdd, 450, 0)
    _link(lerp_hi, emissive, "A")
    _link(rim_mul, emissive, "B")
    _to_prop(emissive, unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    # ---------------------------------------------------------------
    # (E) Opacity: 메인 텍스처 밝기 * fadeAlpha (반투명 페이드).
    #     원본 'alpha' 기본값이 0.0이라 그대로 쓰면 안 보임 → fadeAlpha(0.1) 사용.
    # ---------------------------------------------------------------
    opacity = make(unreal.MaterialExpressionMultiply, 450, 300)
    _link(main_tex, opacity, "A", src_output="R")
    _link(scalar("fadeAlpha", 150, 450, "Alpha"), opacity, "B")
    _to_prop(opacity, unreal.MaterialProperty.MP_OPACITY)

    # 마무리
    MEL.layout_material_expressions(mat)
    MEL.recompile_material(mat)
    EAL.save_loaded_asset(mat)
    unreal.log("불꽃 머티리얼 생성 완료: %s" % full)
    return mat


#/Script/Engine.Material'/Game/Pal/Model/Monster/Kitsunebi/Material/M_Test.M_Test'
if __name__ == "__main__":
    spec = extract_spec_from_file("P:/source/Unreal/ProjectOpenWorld/Content/Pal/Model/Monster/Kitsunebi/Material/M_Pal_Fire.json")  # 실제 경로로 교체
    print_spec(spec)
    build_fire_material(spec, "/Game/Pal/Model/Monster/Kitsunebi/Material", "M_Pal_Fire")
