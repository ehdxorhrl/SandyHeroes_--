#include "stdafx.h"
#include "Material.h"
#include "DescriptorManager.h"
#include "FrameResource.h"
#include "Shader.h"
#include "MeshComponent.h"
#include "Scene.h"
#include "CameraComponent.h"

using namespace file_load_util;


Material::Material(const std::string& name, int shader_type, 
	XMFLOAT4 albedo_color, XMFLOAT3 fresnel_r0, 
	float glossiness, XMFLOAT4 emission_color) 
	: name_(name), shader_type_(shader_type), albedo_color_(albedo_color), 
	fresnel_r0_(fresnel_r0), glossiness_(glossiness), emission_color_(emission_color)
{
}

std::string Material::name() const
{
	return name_;
}

int Material::shader_type() const
{
	return shader_type_;
}

void Material::set_frame_resource_index(int value)
{
	frame_resource_index_ = value;
}

void Material::set_albedo_color(float r, float g, float b, float a)
{
	set_albedo_color(XMFLOAT4{ r, g, b, a });
}

void Material::set_albedo_color(XMFLOAT4 value)
{
	albedo_color_ = value;
}

void Material::set_name(const std::string& value)
{
	name_ = value;
}

void Material::set_shader_type(int value)
{
	shader_type_ = value;
}

void Material::UpdateShaderVariables(ID3D12GraphicsCommandList* command_list, 
	FrameResource* curr_frame_resource, DescriptorManager* descriptor_manager)
{
	// 텍스처 설정
	int srv_offset = descriptor_manager->srv_offset();
	D3D12_GPU_DESCRIPTOR_HANDLE texture_handle; 
	//쉐이더에서 입력된 텍스처를 구별하기 위한 비트마스트
	int texture_mask = 0;
	int texture_root_index = 0;
	for (const auto& const texture : texture_list_)
	{
		texture_handle = descriptor_manager->GetGpuHandle(srv_offset + texture->heap_index);
		switch (texture->type)
		{
		case TextureType::kAlbedoMap:
			texture_mask |= kTextureMaskAlbedo;
			texture_root_index = (int)RootParameterIndex::kAlbedoMap;
			break;
		case TextureType::kSpecGlossMap:
			texture_mask |= kTextureMaskSpecGlos;
			texture_root_index = (int)RootParameterIndex::kSpecGlosMap;
			break;
		case TextureType::kMetallicGlossMap:
			texture_mask |= kTextureMaskMetalGlos;
			texture_root_index = (int)RootParameterIndex::kMetalGlosMap;
			break;
		case TextureType::kEmissionMap:
			texture_mask |= kTextureMaskEmission;
			texture_root_index = (int)RootParameterIndex::kEmissionMap;
			break;
		case TextureType::kNormalMap:
			texture_mask |= kTextureMaskNormal;
			texture_root_index = (int)RootParameterIndex::kNormalMap;
			break;
		case TextureType::kCubeMap:
			texture_mask |= kTextureMaskCube;
			texture_root_index = (int)RootParameterIndex::kCubeMap;
			break;
		default:
			break;
		}
		command_list->SetGraphicsRootDescriptorTable(texture_root_index, texture_handle);
	}

	// 머터리얼 상수버퍼 업데이트
	CBMaterial cb_material = { albedo_color_, fresnel_r0_, glossiness_, emission_color_, texture_mask };
	curr_frame_resource->cb_material->CopyData(frame_resource_index_, cb_material);

	// 머터리얼을 쉐이더에 설정
	UINT cb_material_size = d3d_util::CalculateConstantBufferSize(sizeof(CBMaterial));
	D3D12_GPU_VIRTUAL_ADDRESS material_address =
		curr_frame_resource->cb_material->Resource()->GetGPUVirtualAddress() + (cb_material_size * frame_resource_index_);
	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kMaterial, material_address);

}

void Material::Render(ID3D12GraphicsCommandList* command_list, 
	FrameResource* curr_frame_resource, DescriptorManager* descriptor_manager, CameraComponent* camera, bool bShadow)
{
	//set current material at graphics pipeline
	if(!bShadow)
		UpdateShaderVariables(command_list, curr_frame_resource, descriptor_manager);


	for (const auto& mesh_component : mesh_component_list_)
	{
		if (bShadow)
		{
			mesh_component->Render(this, command_list, curr_frame_resource);
			continue;
		}
		if (camera)
		{
			if (mesh_component->hierarchy_root()->is_in_view_sector())
			{
				if (camera->CollisionCheckByMeshComponent(mesh_component))
				{
					mesh_component->Render(this, command_list, curr_frame_resource);
				}
			}
		}
		else
		{
			mesh_component->Render(this, command_list, curr_frame_resource);
		}
	}
}

int Material::CreateShaderResourceViews(ID3D12Device* device, DescriptorManager* descriptor_manager, int start_index)
{
	int i = start_index;

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.ResourceMinLODClamp = 0.f;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor;

	for (const auto& const texture : texture_list_)
	{
		descriptor = descriptor_manager->GetCpuHandle(i);
		srv_desc.Format = texture->resource->GetDesc().Format;
		srv_desc.Texture2D.MipLevels = texture->resource->GetDesc().MipLevels;
		if (texture->type == TextureType::kCubeMap)
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		}
		else
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		}
		device->CreateShaderResourceView(texture->resource.Get(), &srv_desc, descriptor);
		texture->heap_index = i;
		++i;
	}

	return i;
}

void Material::LoadMaterialFromFile(std::ifstream& file, std::vector<std::unique_ptr<Texture>>& textures)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);

	if (load_token == "<Material>:")
	{
		ReadFromFile<int>(file); // 머터리얼 인덱스
		ReadStringFromFile(file, load_token);
		name_ = load_token;

		ReadStringFromFile(file, load_token);

		if (load_token == "<AlbedoColor>:")
		{
			albedo_color_ = ReadFromFile<XMFLOAT4>(file);
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<EmissiveColor>:")
		{
			emission_color_ = ReadFromFile<XMFLOAT4>(file);
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<SpecularColor>:")
		{
			XMFLOAT4 specular_color = ReadFromFile<XMFLOAT4>(file);
			fresnel_r0_.x = specular_color.x;
			fresnel_r0_.y = specular_color.y;
			fresnel_r0_.z = specular_color.z;

			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<Glossiness>:")
		{
			glossiness_ = ReadFromFile<float>(file);
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<Smoothness>:")
		{
			glossiness_ = ReadFromFile<float>(file);
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<Metallic>:")
		{
			glossiness_ = ReadFromFile<float>(file);
			ReadStringFromFile(file, load_token);
		}

		//texture
		if (load_token == "<AlbedoMap>:")
		{
			ReadStringFromFile(file, load_token); // texture name (ex: Ellen_Body_Albedo)
			if (load_token != "null") 
			{
				auto find_texture = Scene::FindTexture(load_token, textures);
				if (!find_texture)
				{
					textures.push_back(std::make_unique<Texture>());
					textures.back()->name = load_token;
					textures.back()->type = TextureType::kAlbedoMap;
					find_texture = textures.back().get();
				}
				AddTexture(find_texture);
			}
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<SpecGlossMap>:")
		{
			ReadStringFromFile(file, load_token); 
			if (load_token != "null")
			{
				auto find_texture = Scene::FindTexture(load_token, textures);
				if (!find_texture)
				{
					textures.push_back(std::make_unique<Texture>());
					textures.back()->name = load_token;
					textures.back()->type = TextureType::kSpecGlossMap;
					find_texture = textures.back().get();
				}
				AddTexture(find_texture);
			}
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<MetallicGlossMap>:")
		{
			ReadStringFromFile(file, load_token);
			if (load_token != "null")
			{
				auto find_texture = Scene::FindTexture(load_token, textures);
				if (!find_texture)
				{
					textures.push_back(std::make_unique<Texture>());
					textures.back()->name = load_token;
					textures.back()->type = TextureType::kMetallicGlossMap;
					find_texture = textures.back().get();
				}
				AddTexture(find_texture);
			}
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<NormalMap>:")
		{
			ReadStringFromFile(file, load_token);
			if (load_token != "null")
			{
				auto find_texture = Scene::FindTexture(load_token, textures);
				if (!find_texture)
				{
					textures.push_back(std::make_unique<Texture>());
					textures.back()->name = load_token;
					textures.back()->type = TextureType::kNormalMap;
					find_texture = textures.back().get();
				}
				AddTexture(find_texture);
			}
			ReadStringFromFile(file, load_token);
		}

		if (load_token == "<EmissionMap>:")
		{
			ReadStringFromFile(file, load_token);
			if (load_token != "null")
			{
				auto find_texture = Scene::FindTexture(load_token, textures);
				if (!find_texture)
				{
					textures.push_back(std::make_unique<Texture>());
					textures.back()->name = load_token;
					textures.back()->type = TextureType::kEmissionMap;
					find_texture = textures.back().get();
				}
				AddTexture(find_texture);
			}
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "</Material>")
			return;
	}
}

void Material::AddTexture(Texture* texture)
{
	if (texture)
	{
		texture_list_.push_back(texture);
	}
}

std::string Material::GetTextureName(UINT index) const
{
	if (texture_list_.size() <= index)
	{
		return "ErrorIndex";
	}
	int i{ 0 };
	for (const auto& const texture : texture_list_)
	{
		if (i == index)
		{
			return texture->name;
		}
		++i;
	}
}

void Material::AddMeshComponent(MeshComponent* component)
{
	mesh_component_list_.push_back(component);
}

bool Material::DeleteMeshComponent(MeshComponent* component)
{
	auto remove_count = mesh_component_list_.remove(component);

	return remove_count;
}

void Material::CopyMaterialData(Material* material)
{
	albedo_color_ = material->albedo_color_;
	fresnel_r0_ = material->fresnel_r0_;
	glossiness_ = material->glossiness_;
	emission_color_ = material->emission_color_;
}

