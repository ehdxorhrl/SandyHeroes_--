#include "stdafx.h"
#include "UIMesh.h"
#include "Shader.h"
#include "MeshComponent.h"

UIMesh::UIMesh(float screen_x, float screen_y, float screen_width, float screen_height, float z_depth)
    :screen_position_(screen_x, screen_y), ui_size_(screen_width, screen_height)
{
    positions_.reserve(4);

    float left = 0.f;
    float top = 0.f;
    float right = screen_width;
    float bottom = screen_height;

    positions_.push_back(XMFLOAT3{ left, top, z_depth });
    positions_.push_back(XMFLOAT3{ right, top, z_depth });
    positions_.push_back(XMFLOAT3{ left, bottom, z_depth });
    positions_.push_back(XMFLOAT3{ right, bottom, z_depth });

    uvs_.reserve(4);
    uvs_.push_back(XMFLOAT2{ 0.f, 0.f });
    uvs_.push_back(XMFLOAT2{ 1.f, 0.f });
    uvs_.push_back(XMFLOAT2{ 0.f, 1.f });
    uvs_.push_back(XMFLOAT2{ 1.f, 1.f });

    indices_array_.resize(1);

    auto& index_array = indices_array_[0];

    //삼각형 2개
    index_array.reserve(6);

    index_array.insert(index_array.end(), { 0,1,2,1,3,2 });
}

UIMesh::UIMesh(float screen_width, float screen_height, float z_depth)
    :screen_position_(0.f, 0.f), ui_size_(screen_width, screen_height)
{
    positions_.reserve(4);

    float left = 0.f;
    float top = 0.f;
    float right = screen_width;
    float bottom = screen_height;

    positions_.push_back(XMFLOAT3{ left, top, z_depth });
    positions_.push_back(XMFLOAT3{ right, top, z_depth });
    positions_.push_back(XMFLOAT3{ left, bottom, z_depth });
    positions_.push_back(XMFLOAT3{ right, bottom, z_depth });

    uvs_.reserve(4);
    uvs_.push_back(XMFLOAT2{ 0.f, 0.f });
    uvs_.push_back(XMFLOAT2{ 1.f, 0.f });
    uvs_.push_back(XMFLOAT2{ 0.f, 1.f });
    uvs_.push_back(XMFLOAT2{ 1.f, 1.f });

    indices_array_.resize(1);

    auto& index_array = indices_array_[0];

    //삼각형 2개
    index_array.reserve(6);

    index_array.insert(index_array.end(), { 0,1,2,1,3,2 });
}

XMFLOAT3 UIMesh::ScreenToNDC(float x, float y, float width, float height)
{
	return XMFLOAT3{
        (x / width) * 2.0f - 1.0f,
        1.0f - (y / height) * 2.0f,
        0.0f
    };
}

XMFLOAT2 UIMesh::screen_position() const
{
    return screen_position_;
}

XMFLOAT2 UIMesh::ui_size() const
{
    return ui_size_;
}

void UIMesh::UpdateConstantBuffer(FrameResource* curr_frame_resource, int& cb_index)
{
    //메쉬 컴포넌트를 활용하여 오브젝트 CB를 업데이트한다.
    for (auto it = mesh_component_list_.begin(); it != mesh_component_list_.end();)
    {
        auto mesh_component = it->lock();
        if (!mesh_component)
        {
            it = mesh_component_list_.erase(it);
            continue;
        }

        // 그릴 필요 없는 대상에 대해서는 업데이트를 할 필요 없음
    	if (!mesh_component->IsVisible())
    		continue;
    
    	mesh_component->UpdateConstantBuffer(curr_frame_resource, cb_index);
    
    	++cb_index;
    }
}

void UIMesh::UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& cb_index)
{    
    //메쉬 컴포넌트를 활용하여 오브젝트 CB를 업데이트한다.
    for (auto it = mesh_component_list_.begin(); it != mesh_component_list_.end();)
    {
        auto mesh_component = it->lock();
        if (!mesh_component)
        {
            it = mesh_component_list_.erase(it);
            continue;
        }

        mesh_component->UpdateConstantBuffer(curr_frame_resource, cb_index);

        ++cb_index;
    }

}

void UIMesh::Render(ID3D12GraphicsCommandList* command_list, int material_index, FrameResource* curr_frame_resource)
{
    command_list->IASetPrimitiveTopology(primitive_topology_);

    //정점 버퍼 set
    command_list->IASetVertexBuffers(0,
        vertex_buffer_views_.size(), vertex_buffer_views_.data());

    if (material_index < indices_array_.size())
    {
        if (indices_array_[material_index].size())
        {
            command_list->IASetIndexBuffer(&index_buffer_views_[material_index]);
            command_list->DrawIndexedInstanced(indices_array_[material_index].size(), 1, 0, 0, 0);
        }
        else
        {
            command_list->DrawInstanced(positions_.size(), 1, 0, 0);
        }
    }
    else
    {
        if (indices_array_.back().size())
        {
            command_list->IASetIndexBuffer(&index_buffer_views_.back());
            command_list->DrawIndexedInstanced(indices_array_.back().size(), 1, 0, 0, 0);
        }
        else
        {
            command_list->DrawInstanced(positions_.size(), 1, 0, 0);
        }
    }

}
