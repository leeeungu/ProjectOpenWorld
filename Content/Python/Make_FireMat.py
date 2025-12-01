import unreal

# 공통 핸들
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
GLOBAL_PATH = "/Game/Pal/Model/Global/"
# 1) 머티리얼 에셋 생성
factory = unreal.MaterialFactoryNew()
mat = asset_tools.create_asset(
    asset_name="M_Pal_Fire_Auto",
    package_path=GLOBAL_PATH +"Material",
    asset_class=unreal.Material,
    factory=factory
)

# 기본 설정
mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
mat.set_editor_property("use_material_attributes", False)
mat.set_editor_property("used_with_skeletal_mesh", True)

# 2) UV, Time, Panner 등 기본 노드
uv = mel.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -800, -100)

time_expr = mel.create_material_expression(mat, unreal.MaterialExpressionTime, -800, -300)
time_scale = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, -300)
time_scale.set_editor_property("parameter_name", "TimeScale")
time_scale.set_editor_property("default_value", 1.0)

time_mul = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -400, -300)
mel.connect_material_expressions(time_expr, "", time_mul, "A")
mel.connect_material_expressions(time_scale, "", time_mul, "B")

# 3) MainTex용 Panner + Texture
panner_main = mel.create_material_expression(mat, unreal.MaterialExpressionPanner, -600, -100)
# ★ 여기서 바로 uv를 연결하지 말고, 뒤에서 DistortedUV로 다시 덮어쓸 것
mel.connect_material_expressions(uv, "", panner_main, "Coordinate")
mel.connect_material_expressions(time_mul, "", panner_main, "Time")

# 속도/타일링 파라미터
main_u_speed = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -1000, -400)
main_u_speed.set_editor_property("parameter_name" ,"MainUSpeed")
main_u_speed.set_editor_property("default_value", 0.0)

main_v_speed = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -1000, -350)
main_v_speed.set_editor_property("parameter_name", "MainVSpeed")
main_v_speed.set_editor_property("default_value", 1.0)

main_speed = mel.create_material_expression(mat, unreal.MaterialExpressionAppendVector, -800, -400)
mel.connect_material_expressions(main_u_speed, "", main_speed, "A")
mel.connect_material_expressions(main_v_speed, "", main_speed, "B")
mel.connect_material_expressions(main_speed, "", panner_main, "Speed")

main_tex = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, -400, -100)
main_tex.set_editor_property("parameter_name" , "MainTex")
main_tex.set_editor_property("texture", unreal.load_asset(GLOBAL_PATH  +"Textures/T_Cloud_00"))

mel.connect_material_expressions(panner_main, "", main_tex, "UVs")

# 4) NoiseTex 쪽도 유사하게 구성
panner_noise = mel.create_material_expression(mat, unreal.MaterialExpressionPanner, -600, 200)
mel.connect_material_expressions(uv, "", panner_noise, "Coordinate")
mel.connect_material_expressions(time_mul, "", panner_noise, "Time")

noise_tex = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, -400, 200)
noise_tex.set_editor_property("parameter_name",  "NoiseTex")
noise_tex.set_editor_property("texture", unreal.load_asset(GLOBAL_PATH  + "Textures/T_Noise50"))

mel.connect_material_expressions(panner_noise, "", noise_tex, "UVs")

# 4-1) ★ NoiseTex를 이용해 MainTex UV 왜곡 추가 ★

# Distortion 강도 파라미터
distortion_scale = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, 350)
distortion_scale.set_editor_property("parameter_name", "DistortionScale")
distortion_scale.set_editor_property("default_value", 0.1)  # 필요에 따라 조정

# NoiseTex의 R 채널을 Distortion에 사용
distortion_mul = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -400, 350)
mel.connect_material_expressions(noise_tex, "R", distortion_mul, "A")
mel.connect_material_expressions(distortion_scale, "", distortion_mul, "B")

# UV + Distortion
offset_uv = mel.create_material_expression(mat, unreal.MaterialExpressionAdd, -600, 0)
mel.connect_material_expressions(uv, "", offset_uv, "A")
mel.connect_material_expressions(distortion_mul, "", offset_uv, "B")

# Main Panner의 Coordinate를 Distorted UV로 덮어쓰기
mel.connect_material_expressions(offset_uv, "", panner_main, "Coordinate")

# 5) 간단 Emissive: MainTex * Color1
color1 = mel.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -200, -100)
color1.set_editor_property("parameter_name", "Color1")
color1.set_editor_property("default_value", unreal.LinearColor(20.0, 10.3, 0.0, 1.0))

mul_emissive = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, 0, -100)
mel.connect_material_expressions(main_tex, "", mul_emissive, "A")
mel.connect_material_expressions(color1, "", mul_emissive, "B")

emissive_strength = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -200, -250)
emissive_strength.set_editor_property("parameter_name","CaptureEmission")
emissive_strength.set_editor_property("default_value",  1.0)

mul_emissive2 = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, 200, -100)
mel.connect_material_expressions(mul_emissive, "", mul_emissive2, "A")
mel.connect_material_expressions(emissive_strength, "", mul_emissive2, "B")

mel.connect_material_property(mul_emissive2, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

# 6) 간단 Opacity: MainTex 알파 * alpha 파라미터
alpha_param = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -200, 100)
alpha_param.set_editor_property("parameter_name","alpha")
alpha_param.set_editor_property("default_value",  0.5)

mul_alpha = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, 0, 100)
mel.connect_material_expressions(main_tex, "A", mul_alpha, "A")
mel.connect_material_expressions(alpha_param, "", mul_alpha, "B")

mel.connect_material_property(mul_alpha, "", unreal.MaterialProperty.MP_OPACITY)

# 7) 컴파일
mel.recompile_material(mat)
unreal.EditorAssetLibrary.save_asset(mat.get_path_name())
