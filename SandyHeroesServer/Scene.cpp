#include "stdafx.h"
#include "Scene.h"
//#include "InputManager.h"
//#include "InputControllerComponent.h"
#include "GameFramework.h"
#include "MeshComponent.h"
#include "MeshColliderComponent.h"
#include "SkinnedMesh.h"

XMVECTOR Scene::GetPickingPointAtWorld(float sx, float sy, Object* picked_object)
{
	return XMVECTOR();
}

void Scene::AddObject(std::shared_ptr<Object> object)
{
	if(is_updating_objects_)
	{
		add_object_list_.push_back(object);
		return;
	}
	object_list_.push_back(object);
}

void Scene::DeleteObject(std::shared_ptr<Object> object)
{

	if(is_updating_objects_)
	{
		delete_object_list_.push_back(object);
		return;
	}
	for (auto& sector : sectors_)
	{
		sector.DeleteObject(object.get());
	}

	object_list_.remove(object);

}

void Scene::Update(float elapsed_time)
{
	is_updating_objects_ = true;
	for (const std::shared_ptr<Object>& object : object_list_)
	{
		object->Update(elapsed_time);
	}
	is_updating_objects_ = false;

	for (const std::shared_ptr<Object>& object : add_object_list_)
	{
		object_list_.push_back(object);
	}
	add_object_list_.clear();

	for (const std::shared_ptr<Object>& object : delete_object_list_)
	{
		object_list_.remove(object);
	}
	delete_object_list_.clear();

	total_time_ += elapsed_time;
}

void Scene::UpdateObjectWorldMatrix()
{
	for (const std::shared_ptr<Object>& object : object_list_)
	{
		object->UpdateWorldMatrix(nullptr);
	}
}

std::shared_ptr<Object> Scene::FindObject(const std::string& object_name)
{
	auto it = std::find_if(object_list_.begin(), object_list_.end(), [&object_name](const std::shared_ptr<Object>& object) {
		return object->name() == object_name;
		});

	if (it != object_list_.end())
	{
		return *it;
	}

	return nullptr;
}

ModelInfo* Scene::FindModelInfo(const std::string& name)
{
	auto it = std::find_if(model_infos_.begin(), model_infos_.end(), [&name](const std::unique_ptr<ModelInfo>& object) {
		return object.get()->model_name() == name;
		});

	if (it != model_infos_.end())
	{
		return (*it).get();
	}
	return nullptr;
}

void Scene::Initialize(GameFramework* game_framework)
{
	game_framework_ = game_framework;

	BuildMesh();
	BuildObject();
}


using namespace file_load_util;
void Scene::BuildScene(const std::string& scene_name)
{
	std::ifstream scene_file{ "../Resource/Model/" + scene_name + ".bin", std::ios::binary };
	
	int root_object_count = ReadFromFile<int>(scene_file);
	
	std::string load_token;
	
	for (int i = 0; i < root_object_count; ++i)
	{
		ReadStringFromFile(scene_file, load_token);
		if (load_token[0] == '@')
		{
			load_token.erase(0, 1);
			object_list_.push_back(std::shared_ptr<Object>(FindModelInfo(load_token)->GetInstance()));
	
			ReadStringFromFile(scene_file, load_token);
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			object_list_.back()->set_transform_matrix(transfrom);
		}
		else
		{
			std::string object_name = load_token;
	
			ReadStringFromFile(scene_file, load_token); // <Transfrom>
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
	
			model_infos_.push_back(std::make_unique<ModelInfo>("../Resource/Model/" + object_name + ".bin", meshes_, materials_, textures_));
	
			object_list_.push_back(std::shared_ptr<Object>(model_infos_.back()->GetInstance()));
	
			object_list_.back()->set_transform_matrix(transfrom);
	
		}
	}
}

Mesh* Scene::FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes)
{
	auto it = std::find_if(meshes.begin(), meshes.end(), [&mesh_name](const std::unique_ptr<Mesh>& mesh) {
		return mesh.get()->name() == mesh_name;
		});

	if (it != meshes.end())
	{
		return (*it).get();
	}

	return nullptr;
}

std::shared_ptr<Object> Scene::CreatePlayerObject(long long session_id) {
	auto model_info = FindModelInfo("Dog00");
	if (!model_info) return nullptr;

	auto obj = std::shared_ptr<Object>(model_info->GetInstance());
	obj->set_name("Player_" + std::to_string(session_id));
	obj->set_position_vector(0, 0, 0);

	object_list_.push_back(obj);
	return obj;
}

const std::vector<std::unique_ptr<Mesh>>& Scene::meshes() const
{
	return meshes_;
}

Texture* Scene::FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures)
{
	auto it = std::find_if(textures.begin(), textures.end(), [&texture_name](const std::unique_ptr<Texture>& tex) {
		return tex.get()->name == texture_name;
		});

	if (it != textures.end())
	{
		return (*it).get();
	}
	return nullptr;
}


Material* Scene::FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials)
{
	auto it = std::find_if(materials.begin(), materials.end(), [&material_name](const std::unique_ptr<Material>& mat) {
		return mat.get()->name() == material_name;
		});

	if (it != materials.end())
	{
		return (*it).get();
	}

	return nullptr;
}