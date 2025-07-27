#pragma once
#include "MeshComponent.h"

// OBB Ȯ�ο� ����� �޽��� �����ϴ� Ŭ����
// ������ CubeMesh�� ������ 2��° ���ڷ� �ް� �׸� AABB�� �޽��� 3��° ���ڷ� �޴´�.
class DebugMeshComponent :
    public MeshComponent
{
public:
    DebugMeshComponent(Object* owner, Mesh* cube_mesh, Mesh* source_mesh);
    DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingBox& bounds);
    DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingOrientedBox& bounds);
    DebugMeshComponent(const DebugMeshComponent& other);

    virtual Component* GetCopy() override;

private:
    BoundingBox bounds_{};

};

