#include "stdafx.h"
#include "Material.h"
#include "MeshComponent.h"
#include "Scene.h"

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

void Material::DeleteMeshComponent(MeshComponent* component)
{
	mesh_component_list_.remove(component);

}

void Material::CopyMaterialData(Material* material)
{
	albedo_color_ = material->albedo_color_;
	fresnel_r0_ = material->fresnel_r0_;
	glossiness_ = material->glossiness_;
	emission_color_ = material->emission_color_;
}

