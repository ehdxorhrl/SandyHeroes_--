#include "stdafx.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "FrameResource.h"
#include "Material.h"

int Mesh::kCBObjectCurrentIndex = 0;

void Mesh::AddMeshComponent(MeshComponent* mesh_component)
{
	mesh_component_list_.push_back(mesh_component);
}

void Mesh::DeleteMeshComponent(MeshComponent* mesh_component)
{
	auto& delete_target = std::find(mesh_component_list_.begin(), mesh_component_list_.end(), mesh_component);

	mesh_component_list_.erase(delete_target);
}

void Mesh::AddMaterial(Material* material)
{
	materials_.push_back(material);
}

void Mesh::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	if (positions_.size())
	{
		d3d_position_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			positions_.data(), sizeof(XMFLOAT3) * positions_.size(),
			d3d_position_upload_buffer_);


		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_position_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT3) * positions_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT3);
		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (colors_.size())
	{
		d3d_color_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			colors_.data(), sizeof(XMFLOAT4) * colors_.size(),
			d3d_color_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_color_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT4) * colors_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT4);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (uvs_.size())
	{
		d3d_uv_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			uvs_.data(), sizeof(XMFLOAT2) * uvs_.size(),
			d3d_uv_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_uv_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT2) * uvs_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT2);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (normals_.size())
	{
		d3d_normal_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			normals_.data(), sizeof(XMFLOAT3) * normals_.size(),
			d3d_normal_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_normal_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT3) * normals_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT3);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (tangents_.size())
	{
		d3d_tangent_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			tangents_.data(), sizeof(XMFLOAT3) * tangents_.size(),
			d3d_tangent_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_tangent_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT3) * tangents_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT3);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (bi_tangents_.size())
	{
		d3d_bi_tangent_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			bi_tangents_.data(), sizeof(XMFLOAT3) * bi_tangents_.size(),
			d3d_bi_tangent_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_bi_tangent_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT3) * bi_tangents_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT3);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	for (const std::vector<UINT>& index_buffer : indices_array_)
	{
		if (index_buffer.size() == 0)
			continue;
		d3d_index_buffers_.emplace_back();
		d3d_index_upload_buffers_.emplace_back();

		d3d_index_buffers_.back() = d3d_util::CreateDefaultBuffer(device, command_list,
			index_buffer.data(), sizeof(UINT) * index_buffer.size(),
			d3d_index_upload_buffers_.back());

		D3D12_INDEX_BUFFER_VIEW index_buffer_view;
		index_buffer_view.BufferLocation = d3d_index_buffers_.back()->GetGPUVirtualAddress();
		index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
		index_buffer_view.SizeInBytes = sizeof(UINT) * index_buffer.size();

		index_buffer_views_.push_back(index_buffer_view);
	}

}

void Mesh::ReleaseUploadBuffer()
{
	if (d3d_position_upload_buffer_)
		d3d_position_upload_buffer_.Reset();
	if (d3d_color_upload_buffer_)
		d3d_color_upload_buffer_.Reset();
	if (d3d_uv_upload_buffer_)
		d3d_uv_upload_buffer_.Reset();
	if (d3d_normal_upload_buffer_)
		d3d_normal_upload_buffer_.Reset();
	if (d3d_tangent_upload_buffer_)
		d3d_tangent_upload_buffer_.Reset();
	if (d3d_bi_tangent_upload_buffer_)
		d3d_bi_tangent_upload_buffer_.Reset();

	for (ComPtr<ID3D12Resource>& upload_buffer : d3d_index_upload_buffers_)
	{
		upload_buffer.Reset();
	}

}

void Mesh::UpdateConstantBuffer(FrameResource* curr_frame_resource)
{
	//메쉬 컴포넌트를 활용하여 오브젝트 CB를 업데이트한다.
	for (MeshComponent* mesh_component : mesh_component_list_)
	{
		// 그릴 필요 없는 대상에 대해서는 업데이트를 할 필요 없음
		if (!mesh_component->IsVisible())
			continue;

		mesh_component->UpdateConstantBuffer(curr_frame_resource, kCBObjectCurrentIndex);

		kCBObjectCurrentIndex++;
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* command_list, 
	FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager)
{
	FrameResource* curr_frame_resource = frame_resource_manager->curr_frame_resource();
	UINT cb_object_size = d3d_util::CalculateConstantBufferSize(sizeof(CBObject));

	//이 메쉬를 사용하는 오브젝트의 CB 시작 인덱스를 저장한다. 
	int cb_object_start_index = kCBObjectCurrentIndex;

	UpdateConstantBuffer(curr_frame_resource);

	command_list->IASetPrimitiveTopology(primitive_topology_);

	//정점 버퍼 set
	command_list->IASetVertexBuffers(0,
		vertex_buffer_views_.size(), vertex_buffer_views_.data());

	if (indices_array_.size())
	{
		for (int i = 0; i < indices_array_.size(); ++i)
		{
			//머터리얼은 각 서브메쉬에 적용되는 순서대로 저장되어있다.
			materials_[i]->UpdateShaderVariables(command_list, curr_frame_resource, descriptor_manager);
			if (indices_array_[i].size())
			{
				command_list->IASetIndexBuffer(&index_buffer_views_[i]);
				for (int object_index = cb_object_start_index; object_index < kCBObjectCurrentIndex; ++object_index)
				{
					//25.02.23 수정
					//기존 루트 디스크립터 테이블에서 루트 CBV로 변경
					D3D12_GPU_VIRTUAL_ADDRESS cb_object_address =
						curr_frame_resource->cb_object.get()->Resource()->GetGPUVirtualAddress() + object_index * cb_object_size;

					command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kWorldMatrix, cb_object_address);
					command_list->DrawIndexedInstanced(indices_array_[i].size(), 1, 0, 0, 0);
				}
			}
			else
			{
				for (int object_index = cb_object_start_index; object_index < kCBObjectCurrentIndex; ++object_index)
				{
					//25.02.23 수정
					//기존 루트 디스크립터 테이블에서 루트 CBV로 변경
					D3D12_GPU_VIRTUAL_ADDRESS cb_object_address =
						curr_frame_resource->cb_object.get()->Resource()->GetGPUVirtualAddress() + object_index * cb_object_size;

					command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kWorldMatrix, cb_object_address);
					command_list->DrawInstanced(positions_.size(), 1, 0, 0);
				}
			}
		}
	}
	else
	{
		for (int object_index = cb_object_start_index; object_index < kCBObjectCurrentIndex; ++object_index)
		{
			//25.02.23 수정
			//기존 루트 디스크립터 테이블에서 루트 CBV로 변경
			D3D12_GPU_VIRTUAL_ADDRESS cb_object_address =
				curr_frame_resource->cb_object.get()->Resource()->GetGPUVirtualAddress() + object_index * cb_object_size;

			command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kWorldMatrix, cb_object_address);
			command_list->DrawInstanced(positions_.size(), 1, 0, 0);
		}
	}

}

using namespace file_load_util;
void Mesh::LoadMeshFromFile(std::ifstream& file)
{
	int vertex_count = ReadFromFile<int>(file);

	std::string load_token;
	
	ReadStringFromFile(file, load_token);
	name_ = load_token;

	ReadStringFromFile(file, load_token);
	while (load_token != "</Mesh>")
	{
		if (load_token == "<Bounds>:")
		{
			bounds_.Center = ReadFromFile<XMFLOAT3>(file);
			bounds_.Extents = ReadFromFile<XMFLOAT3>(file);
		}
		if (load_token == "<Positions>:")
		{
			positions_.resize(ReadFromFile<int>(file));
			ReadFromFile<XMFLOAT3>(file, positions_.data(), positions_.size());
		}
		else if (load_token == "<TextureCoords>:")
		{
			uvs_.resize(ReadFromFile<int>(file));
			ReadFromFile<XMFLOAT2>(file, uvs_.data(), uvs_.size());
		}
		else if (load_token == "<Normals>:")
		{
			normals_.resize(ReadFromFile<int>(file));
			ReadFromFile<XMFLOAT3>(file, normals_.data(), normals_.size());
		}
		else if (load_token == "<Tangents>:")
		{
			tangents_.resize(ReadFromFile<int>(file));
			ReadFromFile<XMFLOAT3>(file, tangents_.data(), tangents_.size());
		}
		else if (load_token == "<SubSets>:")
		{
			indices_array_.resize(ReadFromFile<int>(file));
			for (auto& indices : indices_array_)
			{
				ReadStringFromFile(file, load_token);
#ifdef _DEBUG
				PrintDebugStringLoadTokenError(name_, load_token, "<SubSet>:");
#endif // _DEBUG
				indices.resize(ReadFromFile<int>(file));
				ReadFromFile<UINT>(file, indices.data(), indices.size());
			}
		}
		ReadStringFromFile(file, load_token);
	}


}

void Mesh::ClearColors()
{
	colors_.clear();
}

void Mesh::ClearUvs()
{
	uvs_.clear();
}

void Mesh::ClearNormals()
{
	normals_.clear();
}

void Mesh::ClearTangents()
{
	tangents_.clear();
}

void Mesh::ResetCBObjectCurrentIndex()
{
	kCBObjectCurrentIndex = 0;
}

int Mesh::shader_type() const
{
	return shader_type_;
}

std::string Mesh::name() const
{
	return name_;
}

const std::list<MeshComponent*>& Mesh::mesh_component_list() const
{
	return mesh_component_list_;
}

BoundingBox Mesh::bounds() const
{
	return bounds_;
}

const std::vector<XMFLOAT3>& Mesh::positions() const
{
	return positions_;
}

const std::vector<std::vector<UINT>>& Mesh::indices_array() const
{
	return indices_array_;
}

D3D12_PRIMITIVE_TOPOLOGY Mesh::primitive_topology() const
{
	return primitive_topology_;
}

void Mesh::set_shader_type(int value)
{
	shader_type_ = value;
}

void Mesh::set_name(const std::string& name)
{
	name_ = name;
}
