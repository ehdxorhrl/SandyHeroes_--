#pragma once
#include "Mesh.h"
class SkinnedMesh :
    public Mesh
{
public:
    virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
    virtual void ReleaseUploadBuffer() override;

    virtual void UpdateConstantBuffer(FrameResource* curr_frame_resource) override;

    virtual void Render(ID3D12GraphicsCommandList* command_list,
        FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager) override;

    void LoadSkinnedMeshFromFile(std::ifstream& file);

    static void ResetCBSkinnedMeshObjectCurrentIndex();

private:
    static int kCBSkinnedMeshObjectCurrentIndex;

    int bones_per_vertex_ = 4;

    std::vector<std::string> bone_names_;

    std::vector<XMFLOAT4X4> bone_offsets_;
    ComPtr<ID3D12Resource> d3d_bone_offset_buffer_ = nullptr;
    ComPtr<ID3D12Resource> d3d_bone_offset_upload_buffer_ = nullptr;

    std::vector<XMINT4> bone_indices_;
    ComPtr<ID3D12Resource> d3d_bone_index_buffer_ = nullptr;
    ComPtr<ID3D12Resource> d3d_bone_index_upload_buffer_ = nullptr;

    std::vector<XMFLOAT4> bone_weights_;
    ComPtr<ID3D12Resource> d3d_bone_weight_buffer_ = nullptr;
    ComPtr<ID3D12Resource> d3d_bone_weight_upload_buffer_ = nullptr;


};

