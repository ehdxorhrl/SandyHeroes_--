#pragma once
#include "Component.h"
#include "Mesh.h"

class Scene;
struct FrameResource;

// 오브젝트에 메쉬 기능을 추가해 주는 클래스
// 
class MeshComponent :
    public Component
{
public:
    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(Object* owner, Mesh* mesh, Material* material);
    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& rhs);

    virtual ~MeshComponent() override;

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index);
    virtual void UpdateConstantBufferForShadow(FrameResource* current_frame_resource, int cb_index);

    virtual void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

    void AddMaterial(Material* material);

    bool ChangeMaterial(int index, Material* material);

    bool IsVisible() const;

    void set_is_visible(bool value);
	void set_is_in_view_frustum(bool value);

	bool is_in_view_frustum() const;

    Mesh* GetMesh() const;

    void set_mesh(Mesh* mesh);

    UINT constant_buffer_index() const;
protected:
    UINT constant_buffer_index_{};

    Mesh* mesh_ = nullptr;

    std::vector<Material*> materials_;

    bool is_visible_ = true;

	bool is_in_view_frustum_ = false;
};

