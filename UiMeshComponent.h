#pragma once
#include "MeshComponent.h"

class UiMeshComponent :
    public MeshComponent
{
public:
    UiMeshComponent(Object* owner, Mesh* mesh);
    UiMeshComponent(Object* owner, Mesh* mesh, Material* material, Scene* scene);
    UiMeshComponent(const UiMeshComponent& other);
    UiMeshComponent& operator=(const UiMeshComponent& rhs);

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index);

    virtual void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

    //getter
    std::string name() const;

    //setter
    void set_name(const std::string& value);
    void set_is_static(bool value);
    void set_ui_ratio(XMFLOAT2 value);

private:
    std::string name_{ "UiMeshComponent" };

    Scene* scene_{};

    XMFLOAT2 ui_ratio_{ 1.f, 1.f }; //Ui width, height ratio
    bool is_static_{ false }; //static ui use UiMesh screen_position at screen_offset
    bool is_in_screen_{ true };

};


