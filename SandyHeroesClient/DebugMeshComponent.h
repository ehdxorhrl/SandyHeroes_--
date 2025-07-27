#pragma once
#include "MeshComponent.h"

// OBB 확인용 디버그 메쉬를 렌더하는 클래스
// 기존의 CubeMesh를 생성자 2번째 인자로 받고 그릴 AABB나 메쉬를 3번째 인자로 받는다.
class DebugMeshComponent :
    public MeshComponent
{
public:
    DebugMeshComponent(Object* owner, Mesh* cube_mesh, Mesh* source_mesh);
    DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingBox& bounds);
    DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingOrientedBox& bounds);
    DebugMeshComponent(const DebugMeshComponent& other);

    virtual Component* GetCopy() override;
    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index) override;

private:
    BoundingBox bounds_{};

};

