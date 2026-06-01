# -*- coding: utf-8 -*-
"""
UE 5.4 / Python Editor Scripting
기준 소캣(reference socket)을 기준으로 그 로컬 공간에 offset만큼 떨어진
새 소캣을 SkeletalMesh에 추가한다.

[전제]
- 언리얼에서 소캣은 '본(bone)'에만 부착된다. 소캣을 다른 소캣의 부모로 둘 수 없다.
- 따라서 새 소캣은 기준 소캣과 '같은 본'에 부착되고,
  트랜스폼은 compose_transforms(offset, ref) = offset을 기준 소캣 로컬 공간에서 적용한 결과로 만든다.
- offset을 기본값(0/0/1)으로 두면 기준 소캣과 동일한 위치에 새 소캣이 생긴다.

[검증 완료 API - UE5.4 Python]
- SkeletalMesh.find_socket_and_index(name) -> (SkeletalMeshSocket, index)
- SkeletalMesh.add_socket(socket, add_to_skeleton=False)
- SkeletalMeshSocket.set_editor_property('socket_name'/'bone_name'/...)  # docs상 [Read-Only]지만 EditAnywhere라 set 가능
- unreal.Transform(location, rotation, scale)  # rotation 인자는 Rotator
- unreal.MathLibrary.compose_transforms(a, b)  # = a*b, "a 먼저 → b"
"""

import unreal


def _resolve_skeletal_mesh(mesh):
    """경로 문자열 또는 에셋 오브젝트를 받아 USkeletalMesh로 정규화. 실패 시 None."""
    asset = unreal.load_asset(mesh) if isinstance(mesh, str) else mesh
    if not isinstance(asset, unreal.SkeletalMesh):
        unreal.log_error("[Socket] SkeletalMesh가 아님: {}".format(mesh))
        return None
    return asset


def add_socket_relative_to_socket(
    mesh,
    reference_socket_name,
    new_socket_name,
    offset_location=unreal.Vector(0.0, 0.0, 0.0),
    offset_rotation=unreal.Rotator(0.0, 0.0, 0.0),
    offset_scale=unreal.Vector(1.0, 1.0, 1.0),
    add_to_skeleton=False,
    save=True,
):
    """기준 소캣을 기준으로 새 소캣을 추가한다.

    :param mesh: USkeletalMesh 또는 에셋 경로(str). 예) '/Game/Pal/SK_Pal'
    :param reference_socket_name: 기준이 되는 기존 소캣 이름
    :param new_socket_name: 새로 만들 소캣 이름 (중복 시 생성하지 않음)
    :param offset_location/rotation/scale: 기준 소캣 '로컬 공간' 기준 오프셋
    :param add_to_skeleton: False=Mesh Socket(메시에 저장) / True=Skeleton Socket(스켈레톤에 저장)
    :param save: 성공 시 에셋 저장 여부
    :return: 생성된 USkeletalMeshSocket, 실패 시 None
    """
    mesh = _resolve_skeletal_mesh(mesh)
    if mesh is None:
        return None

    # 1) 기준 소캣 확인
    ref_socket, _ = mesh.find_socket_and_index(reference_socket_name)
    if ref_socket is None:
        unreal.log_error("[Socket] 기준 소캣을 찾을 수 없음: {}".format(reference_socket_name))
        return None

    # 2) 중복 방지
    existing, _ = mesh.find_socket_and_index(new_socket_name)
    if existing is not None:
        unreal.log_error("[Socket] 이미 존재하는 소캣 이름: {}".format(new_socket_name))
        return None

    # 3) 트랜스폼 합성: 기준 소캣(본 기준 상대) 위에 offset을 기준 소캣 로컬 공간으로 적용
    ref_xform = unreal.Transform(
        ref_socket.get_editor_property("relative_location"),
        ref_socket.get_editor_property("relative_rotation"),
        ref_socket.get_editor_property("relative_scale"),
    )
    offset_xform = unreal.Transform(offset_location, offset_rotation, offset_scale)
    new_xform = unreal.MathLibrary.compose_transforms(offset_xform, ref_xform)  # offset 먼저 → ref

    # 4) 소캣 생성 및 프로퍼티 설정 (Outer는 반드시 메시)
    socket = unreal.SkeletalMeshSocket(mesh)
    socket.set_editor_property("socket_name", new_socket_name)
    socket.set_editor_property("bone_name", ref_socket.get_editor_property("bone_name"))
    socket.set_editor_property("relative_location", new_xform.translation)
    socket.set_editor_property("relative_rotation", new_xform.rotation.rotator())  # Quat -> Rotator
    socket.set_editor_property("relative_scale", new_xform.scale3d)

    # 5) 메시에 부착 (add_to_skeleton=True면 스켈레톤 소캣으로 승격)
    mesh.add_socket(socket, add_to_skeleton)

    # 6) 저장
    if save:
        asset_sys = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
        asset_sys.save_loaded_asset(mesh)
        if add_to_skeleton and mesh.skeleton:
            asset_sys.save_loaded_asset(mesh.skeleton)

    unreal.log("[Socket] 생성 완료: '{}' (bone={}, add_to_skeleton={})".format(
        new_socket_name, socket.get_editor_property("bone_name"), add_to_skeleton))
    return socket


if __name__ == "__main__":
    # 사용 예시 — 경로/이름/오프셋만 맞춰서 사용
    add_socket_relative_to_socket(
        mesh="/Game/Pal/Mesh/SK_Pal",          # pal 스켈레탈 메시 경로
        reference_socket_name="hand_r_socket",  # 기준 소캣
        new_socket_name="hand_r_weapon_tip",    # 새 소캣
        offset_location=unreal.Vector(10.0, 0.0, 0.0),  # 기준 소캣 로컬 +X로 10cm
    )
