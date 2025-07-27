#include "stdafx.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "XMathUtil.h"

Mesh::~Mesh()
{
	for (auto& mesh_component : mesh_component_list_)
	{
		mesh_component->set_mesh(nullptr);
	}
}

void Mesh::AddMeshComponent(MeshComponent* mesh_component)
{
	mesh_component_list_.push_back(mesh_component);
}

void Mesh::DeleteMeshComponent(MeshComponent* mesh_component)
{
	mesh_component_list_.remove(mesh_component);
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

const std::vector<XMFLOAT3>& Mesh::positions() const
{
    return positions_;
}

const std::vector<std::vector<UINT>>& Mesh::indices_array() const
{
    return indices_array_;
}

BoundingBox Mesh::bounds() const
{
    return bounds_;
}

std::string Mesh::name() const
{
    return name_;
}

void Mesh::set_name(const std::string& name)
{
    name_ = name;
}

D3D_PRIMITIVE_TOPOLOGY Mesh::primitive_topology() const
{
    return primitive_topology_;
}

const std::list<MeshComponent*>& Mesh::mesh_component_list() const
{
    return mesh_component_list_;
}