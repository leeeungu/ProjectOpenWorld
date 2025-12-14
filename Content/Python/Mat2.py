import unreal

#/Script/Engine.Material'/Game/Building/Widget/Material/M_BuildingMode.M_BuildingMode'
def create_radial_sector_select_material(
    package_path="/Game/Building/Widget/Material/",
    material_name="M_RadialSectorSelect"
):
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    # 1) Material Asset 생성
    mat_factory = unreal.MaterialFactoryNew()
    material = asset_tools.create_asset(material_name, package_path, unreal.Material, mat_factory)

    # 편집용 라이브러리
    mel = unreal.MaterialEditingLibrary

    # 2) 파라미터/상수 준비
    # Scalar Params
    p_sector_count = mel.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -800, -200)
    p_sector_count.set_editor_property("parameter_name", "SectorCount")
    p_sector_count.set_editor_property("default_value", 8.0)

    p_selected_idx = mel.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -800, -80)
    p_selected_idx.set_editor_property("parameter_name", "SelectedIndex")
    p_selected_idx.set_editor_property("default_value", 0.0)

    # Vector Params (Color)
    p_base_color = mel.create_material_expression(material, unreal.MaterialExpressionVectorParameter, -800, 120)
    p_base_color.set_editor_property("parameter_name", "BaseColor")
    p_base_color.set_editor_property("default_value", unreal.LinearColor(0.1, 0.1, 0.1, 1.0))

    p_highlight_color = mel.create_material_expression(material, unreal.MaterialExpressionVectorParameter, -800, 260)
    p_highlight_color.set_editor_property("parameter_name", "HighlightColor")
    p_highlight_color.set_editor_property("default_value", unreal.LinearColor(1.0, 0.2, 0.2, 1.0))

    # Center (Vector2): Constant2Vector로 기본 0.5,0.5 제공
    c_center = mel.create_material_expression(material, unreal.MaterialExpressionConstant2Vector, -800, -420)
    c_center.set_editor_property("r", 0.5)
    c_center.set_editor_property("g", 0.5)

    # Constants PI / 2PI
    c_pi = mel.create_material_expression(material, unreal.MaterialExpressionConstant, -520, -420)
    c_pi.set_editor_property("r", 3.14159265)

    c_two_pi = mel.create_material_expression(material, unreal.MaterialExpressionConstant, -520, -360)
    c_two_pi.set_editor_property("r", 6.28318530)

    # 3) UV 및 P = UV - Center
    texcoord = mel.create_material_expression(material, unreal.MaterialExpressionTextureCoordinate, -1040, -420)
    texcoord.set_editor_property("coordinate_index", 0)

    sub_uv_center = mel.create_material_expression(material, unreal.MaterialExpressionSubtract, -520, -520)
    mel.connect_material_expressions(texcoord, "", sub_uv_center, "A")
    mel.connect_material_expressions(c_center, "", sub_uv_center, "B")

    # P.x, P.y 분리 (ComponentMask)
    mask_x = mel.create_material_expression(material, unreal.MaterialExpressionComponentMask, -260, -600)
    mask_x.set_editor_property("r", True)
    mask_x.set_editor_property("g", False)
    mask_x.set_editor_property("b", False)
    mask_x.set_editor_property("a", False)
    mel.connect_material_expressions(sub_uv_center, "", mask_x, "Input")

    mask_y = mel.create_material_expression(material, unreal.MaterialExpressionComponentMask, -260, -520)
    mask_y.set_editor_property("r", False)
    mask_y.set_editor_property("g", True)
    mask_y.set_editor_property("b", False)
    mask_y.set_editor_property("a", False)
    mel.connect_material_expressions(sub_uv_center, "", mask_y, "Input")

    # 4) angleRad = atan2(P.y, P.x)
    # UE 머티리얼 노드: Arctangent2( Y, X )
    atan2 = mel.create_material_expression(material, unreal.MaterialExpressionArctangent2, 0, -560)
    mel.connect_material_expressions(mask_y, "", atan2, "Y")
    mel.connect_material_expressions(mask_x, "", atan2, "X")

    # angle01 = (angleRad + PI) / (2PI)
    add_pi = mel.create_material_expression(material, unreal.MaterialExpressionAdd, 220, -560)
    mel.connect_material_expressions(atan2, "", add_pi, "A")
    mel.connect_material_expressions(c_pi, "", add_pi, "B")

    div_two_pi = mel.create_material_expression(material, unreal.MaterialExpressionDivide, 440, -560)
    mel.connect_material_expressions(add_pi, "", div_two_pi, "A")
    mel.connect_material_expressions(c_two_pi, "", div_two_pi, "B")

    # 5) idx = floor(angle01 * SectorCount)
    mul_sector = mel.create_material_expression(material, unreal.MaterialExpressionMultiply, 660, -560)
    mel.connect_material_expressions(div_two_pi, "", mul_sector, "A")
    mel.connect_material_expressions(p_sector_count, "", mul_sector, "B")

    floor_idx = mel.create_material_expression(material, unreal.MaterialExpressionFloor, 880, -560)
    mel.connect_material_expressions(mul_sector, "", floor_idx, "Input")

    # 6) isSelected = 1 - step(0.5, abs(idx - SelectedIndex))
    sub_idx = mel.create_material_expression(material, unreal.MaterialExpressionSubtract, 1100, -560)
    mel.connect_material_expressions(floor_idx, "", sub_idx, "A")
    mel.connect_material_expressions(p_selected_idx, "", sub_idx, "B")

    abs_node = mel.create_material_expression(material, unreal.MaterialExpressionAbs, 1320, -560)
    mel.connect_material_expressions(sub_idx, "", abs_node, "Input")

    c_half = mel.create_material_expression(material, unreal.MaterialExpressionConstant, 1320, -460)
    c_half.set_editor_property("r", 0.5)

    step_node = mel.create_material_expression(material, unreal.MaterialExpressionStep, 1540, -560)
    # Step(A, X): returns X >= A ? 1 : 0  (UE 구현 기준)
    # A=0.5, X=abs(...)
    mel.connect_material_expressions(c_half, "", step_node, "Y")  # 임계값 쪽 핀명이 엔진 버전에 따라 다를 수 있음
    mel.connect_material_expressions(abs_node, "", step_node, "X")

    one_minus = mel.create_material_expression(material, unreal.MaterialExpressionOneMinus, 1760, -560)
    mel.connect_material_expressions(step_node, "", one_minus, "Input")

    # 7) Lerp(BaseColor, HighlightColor, isSelected)
    lerp = mel.create_material_expression(material, unreal.MaterialExpressionLinearInterpolate, 1760, 120)
    mel.connect_material_expressions(p_base_color, "", lerp, "A")
    mel.connect_material_expressions(p_highlight_color, "", lerp, "B")
    mel.connect_material_expressions(one_minus, "", lerp, "Alpha")

    # 8) Material Output 연결
    mel.connect_material_property(lerp, "", unreal.MaterialProperty.MP_BASE_COLOR)

    # 정리/컴파일
    mel.layout_material_expressions(material)
    mel.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material)

    return material


# 실행 예시
if __name__ == "__main__":
	create_radial_sector_select_material("/Game/Building/Widget/Material/", "M_RadialSectorSelect")
