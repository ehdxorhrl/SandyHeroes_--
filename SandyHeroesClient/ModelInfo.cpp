#include "stdafx.h"
#include "ModelInfo.h"
#include "Object.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "MeshComponent.h"
#include "SkinnedMeshComponent.h"
#include "Shader.h"
#include "Material.h"
#include "AnimatorComponent.h"
#include "Scene.h"
#include "PlayerAnimationState.h"
#include "BoxColliderComponent.h"
#include "DebugMeshComponent.h"
#include "GroundColliderComponent.h"
#include "WallColliderComponent.h"

ModelInfo::ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
	std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures)
{
	LoadModelInfoFromFile(file_name, meshes, materials, textures);
}

ModelInfo::~ModelInfo()
{
	if (hierarchy_root_)
		delete hierarchy_root_;
}

std::string ModelInfo::model_name() const
{
	return model_name_;
}

using namespace file_load_util;
void ModelInfo::LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
	std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures)
{
	size_t last_slash = file_name.find_last_of("/\\");
	size_t last_dot = file_name.find_last_of('.');
	model_name_ = file_name.substr(last_slash + 1, last_dot - last_slash - 1);

	std::ifstream model_file{ file_name, std::ios::binary };

	std::string load_token; 

	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<Hierarchy>:");
#endif //_DEBUG

	hierarchy_root_ = LoadFrameInfoFromFile(model_file, meshes, materials, textures);

	hierarchy_root_->set_position_vector(0, 0, 0); //모델 정보는 0 0 0에 위치

	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Hierarchy>");
#endif //_DEBUG

	if (model_file.peek() == EOF)
	{
		return;
	}

	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<Animation>:");
#endif //_DEBUG

	ReadStringFromFile(model_file, load_token);
	root_bone_name_ = load_token;

	LoadAnimationInfoFromFile(model_file);

	if (animation_sets_.size())
	{
		AnimatorComponent* animator = new AnimatorComponent(hierarchy_root_, animation_sets_, frame_names_, root_bone_name_, new PlayerAnimationState);
		hierarchy_root_->AddComponent(animator);
	}


	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Animation>");
#endif //_DEBUG

}

Object* ModelInfo::LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes,
	std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<Frame>:");
#endif //_DEBUG

	Object* frame = new Object;

	//object name
	ReadStringFromFile(file, load_token);
	frame->set_name(load_token);

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<TransformMatrix>:");
#endif //_DEBUG
	XMFLOAT4X4 transform = ReadFromFile<XMFLOAT4X4>(file);
	frame->set_transform_matrix(transform);

	ReadStringFromFile(file, load_token);

	if (load_token == "<BoxCollider>:")
	{
		BoundingBox box;
		box.Center = ReadFromFile<XMFLOAT3>(file);
		box.Extents = ReadFromFile<XMFLOAT3>(file);
		frame->AddComponent(new BoxColliderComponent(frame, box));

		ReadStringFromFile(file, load_token);
	}
	if (load_token == "<MapNode>:")
	{
		XMFLOAT3 position = ReadFromFile<XMFLOAT3>(file);
		int id = ReadFromFile<int>(file);
		int neighbors_count = ReadFromFile<int>(file);
		std::vector<int> neighbors;
		neighbors.resize(neighbors_count);
		ReadFromFile<int>(file, neighbors.data(), neighbors_count);

		ReadStringFromFile(file, load_token);
	}

	if (load_token == "<Mesh>:")
	{
		Mesh* mesh = new Mesh;
		mesh->LoadMeshFromFile(file);

		Mesh* find_mesh = Scene::FindMesh(mesh->name(), meshes);
		if (find_mesh)
		{
			delete mesh;
			mesh = find_mesh;
		}
		else
		{
			meshes.emplace_back();
			meshes.back().reset(mesh);
		}

		MeshComponent* mesh_component = new MeshComponent(frame, mesh);
		mesh->DeleteMeshComponent(mesh_component);	//모델 정보는 그려지면 안되기 때문에 메쉬의 컴포넌트 리스트에서 제외함
		frame->AddComponent(mesh_component);

		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "<Materials>:");
#endif //_DEBUG
		int material_count = ReadFromFile<int>(file);
		for (int i = 0; i < material_count; ++i)
		{
			Material* material = new Material();
			material->LoadMaterialFromFile(file, textures);
			material->set_shader_type((int)ShaderType::kStandardMesh);

			Material* find_material = Scene::FindMaterial(material->name(), materials);
			if (find_material && find_material->shader_type() != (int)ShaderType::kStandardSkinnedMesh)
			{
				delete material;
				material = find_material;
			}
			else
			{
				if (find_material)
				{
					material->set_name(find_material->name() + "_copy");
				}

				materials.emplace_back();
				materials.back().reset(material);
			}

			mesh_component->AddMaterial(material);
			material->DeleteMeshComponent(mesh_component);
		}
		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "</Materials>");
#endif //_DEBUG

		ReadStringFromFile(file, load_token);

		if (load_token == "<GroundCollider>:")
		{
			if (mesh)
			{
				GroundColliderComponent* mesh_collider = new GroundColliderComponent(frame);
				mesh_collider->set_mesh(mesh);
				frame->AddComponent(mesh_collider);
			}
			ReadStringFromFile(file, load_token);
		}
		if (load_token == "<WallCollider>:")
		{
			if (mesh)
			{
				WallColliderComponent* wall_collider = new WallColliderComponent(frame);
				wall_collider->set_mesh(mesh);
				auto box = Object::GetComponent<BoxColliderComponent>(frame);
				if(box)
				{
					wall_collider->set_box_collider(box);
				}
				frame->AddComponent(wall_collider);
			}
			ReadStringFromFile(file, load_token);
		}

	}
	else if (load_token == "<SkinningInfo>:")
	{
		SkinnedMesh* skinned_mesh = new SkinnedMesh;
		skinned_mesh->LoadSkinnedMeshFromFile(file);

		Mesh* find_mesh = Scene::FindMesh(skinned_mesh->name(), meshes);
		if (find_mesh)
		{
			delete skinned_mesh;
			skinned_mesh = static_cast<SkinnedMesh*>(find_mesh);
		}
		else
		{
			meshes.emplace_back();
			meshes.back().reset(skinned_mesh);
		}

		SkinnedMeshComponent* skinned_mesh_component = new SkinnedMeshComponent(frame, skinned_mesh);
		skinned_mesh->DeleteMeshComponent(skinned_mesh_component);
		frame->AddComponent(skinned_mesh_component);

		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "<Materials>:");
#endif //_DEBUG
		int material_count = ReadFromFile<int>(file);
		for (int i = 0; i < material_count; ++i)
		{
			Material* material = new Material();
			material->LoadMaterialFromFile(file, textures);
			material->set_shader_type((int)ShaderType::kStandardSkinnedMesh);

			Material* find_material = Scene::FindMaterial(material->name(), materials);
			if (find_material)
			{
				delete material;
				material = find_material;
			}
			else
			{
				materials.emplace_back();
				materials.back().reset(material);
			}

			skinned_mesh_component->AddMaterial(material);
			material->DeleteMeshComponent(skinned_mesh_component);
		}
		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "</Materials>");
#endif //_DEBUG

		ReadStringFromFile(file, load_token);

	}
	
	if (load_token == "<Children>:")
	{
		int child_count = ReadFromFile<int>(file);
		for (int i = 0; i < child_count; ++i)
		{
			frame->AddChild(LoadFrameInfoFromFile(file, meshes, materials, textures));
		}

		ReadStringFromFile(file, load_token);
	}

#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Frame>");
#endif //_DEBUG


	return frame;
}

void ModelInfo::LoadAnimationInfoFromFile(std::ifstream& file)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<AnimationSets>:");
#endif //_DEBUG

	int animation_set_count = ReadFromFile<int>(file);
	animation_sets_.resize(animation_set_count);

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<FrameNames>:");
#endif //_DEBUG

	int frame_count = ReadFromFile<int>(file);
	frame_names_.resize(frame_count);
	for (std::string& name : frame_names_)
	{
		ReadStringFromFile(file, load_token);
		name = load_token;
	}


	for (std::unique_ptr<AnimationSet>& animation_set : animation_sets_)
	{
		ReadStringFromFile(file, load_token);
#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "<AnimationSet>:");
#endif //_DEBUG

		ReadFromFile<int>(file); //index

		animation_set = std::make_unique<AnimationSet>();
		animation_set->LoadAnimationSetFromFile(file, frame_count);
	}

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</AnimationSets>");
#endif //_DEBUG

}

Object* ModelInfo::GetInstance() const
{
	Object* r_value = Object::DeepCopy(hierarchy_root_);

	return r_value;
}

Object* ModelInfo::hierarchy_root() const
{
	return hierarchy_root_;
}

void ModelInfo::set_hierarchy_root(Object* root)
{
	if (hierarchy_root_)
	{
		delete hierarchy_root_;
	}
	hierarchy_root_ = root;
}
