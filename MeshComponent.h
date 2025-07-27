#pragma once
#include "Component.h"
#include "Mesh.h"

struct FrameResource;

// 오브젝트에 메쉬 기능을 추가해 주는 클래스
// 
class MeshComponent :
    public Component
{
public:
    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& rhs);

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index);

    bool IsVisible() const;

    void set_is_visible(bool value);

    Mesh* GetMesh() const;
protected:
    Mesh* mesh_ = nullptr;

    bool is_visible_ = true;
};

