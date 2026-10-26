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
    DECLARE_COMPONENT(MeshComponent, Component)

    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh);
    MeshComponent(Object* owner, Mesh* mesh, Material* material);
    MeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh, Material* material);
    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& rhs);

    virtual ~MeshComponent() override;

    virtual Component* GetCopy() override;

	void Update(float elapsed_time) override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource);
	void UpdateConstantBufferForBillboard(FrameResource* current_frame_resource);

    virtual void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

    void AddMaterial(Material* material);

    bool ChangeMaterial(int index, Material* material);

    bool IsVisible() const;

    void set_is_visible(bool value);
	void set_is_in_view_frustum(bool value);
    void set_is_in_shadow_map_obb(bool value) { if(is_using_shadow_map_obb_culling) is_in_shadow_map_obb_ = value; }
	void set_is_using_view_frustum_culling(bool value) { is_using_view_frustum_culling = value; }
	void set_is_using_shadow_map_obb_culling(bool value) { is_using_shadow_map_obb_culling = value; }

	bool is_in_view_frustum() const;
    bool is_in_shadow_map_obb() const { if (is_using_shadow_map_obb_culling) return is_in_shadow_map_obb_; else true; }

    Mesh* GetMesh() const;

    Material* GetMaterial(int index = 0) const;

    void set_mesh(Mesh* mesh);

    UINT constant_buffer_index() const;

	MeshType mesh_type() const { return mesh_type_; }
	int GetMaterialIndex(Material* material) const;
protected:
	MeshType mesh_type_ = MeshType::kStaticMesh;

    UINT constant_buffer_index_{};

    Mesh* mesh_ = nullptr;

    std::vector<Material*> materials_;

    bool is_visible_ = true;

	bool is_in_view_frustum_ = false;
    bool is_in_shadow_map_obb_ = false;

	//뷰 프러스텀 컬링이 적용되는지 여부. 이 값이 false이면 컬링이 적용되지 않는다.(항상 렌더링 됨)
	bool is_using_view_frustum_culling = true;
	bool is_using_shadow_map_obb_culling = true;
};

