#pragma once
#include "MeshComponent.h"

enum class UiLayer{ kZero, kOne, kTwo };

class UiMeshComponent :
    public MeshComponent
{
public:
    UiMeshComponent(Object* owner, Mesh* mesh);
    UiMeshComponent(Object* owner, Mesh* mesh, Material* material, Scene* scene);
    UiMeshComponent(const UiMeshComponent& other);
    UiMeshComponent& operator=(const UiMeshComponent& rhs);
    ~UiMeshComponent() {}

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index);

    virtual void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

    //getter
    std::string name() const;

    //setter
    void set_name(const std::string& value);
    void set_is_static(bool value);
    void set_ui_ratio(XMFLOAT2 value);
	void set_ui_layer(UiLayer value) { ui_layer_ = value; }
	void set_texture_offset(XMFLOAT2 value) { texture_offset_ = value; }
	void set_gage_value(XMFLOAT2 value) { gage_value_ = value; }
	void set_position_offset(XMFLOAT2 value) { position_offset_ = value; }
    void set_alpha(float alpha) { alpha_ = std::clamp(alpha, 0.0f, 1.0f); }
private:
    std::string name_{ "UiMeshComponent" };

    Scene* scene_{};

	XMFLOAT2 position_offset_{ 0.f, 0.f }; //Ui position offset from pivot in pixel

    XMFLOAT2 ui_ratio_{ 1.f, 1.f }; //Ui width, height ratio
    bool is_static_{ false }; //static ui use UiMesh screen_position at screen_offset
    bool is_in_screen_{ true };
	XMFLOAT2 texture_offset_{ 0.f, 0.f }; //texture offset for ui texture
	UiLayer ui_layer_{ UiLayer::kZero }; //ui layer
	XMFLOAT2 gage_value_{ 1.f, 1.f }; //ui gage value (0.0 ~ 1.0)

    float alpha_ = 1.0f;
};


