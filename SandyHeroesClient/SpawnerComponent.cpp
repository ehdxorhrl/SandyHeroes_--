#include "stdafx.h"
#include "SpawnerComponent.h"
#include "Scene.h"

SpawnerComponent::SpawnerComponent(Object* owner, Scene* scene) : Component(owner), scene_(scene)
{
}

SpawnerComponent::SpawnerComponent(Object* owner, Scene* scene, ModelInfo* model_info) : Component(owner), model_info_(model_info), scene_(scene)
{
}

SpawnerComponent::SpawnerComponent(const SpawnerComponent& other) : Component(other),
spawn_count_(other.spawn_count_), spawn_time_(other.spawn_time_), spawn_cool_time_(other.spawn_cool_time_), 
scene_(other.scene_), model_info_(other.model_info_)
{
	for (auto& component : other.component_list_)
	{
		component_list_.emplace_back();
		component_list_.back().reset(component->GetCopy());
	}
}

SpawnerComponent::SpawnerComponent(const SpawnerComponent&& other) : Component(other), 
spawn_count_(other.spawn_count_), spawn_time_(other.spawn_time_), spawn_cool_time_(other.spawn_cool_time_), 
scene_(other.scene_), model_info_(other.model_info_)
{
	for (auto& component : other.component_list_)
	{
		component_list_.emplace_back();
		component_list_.back().reset(component->GetCopy());
	}

}

Component* SpawnerComponent::GetCopy()
{
	return new SpawnerComponent(*this);
}

void SpawnerComponent::Update(float elapsed_time)
{
	if (!scene_ || !is_spawn_)
	{
		return;
	}	
	if (spawn_count_ <= 0)
	{
		is_spawn_ = false;
		return;
	}
	spawn_time_ -= elapsed_time;
	if (spawn_time_ <= 0.f)
	{
		ForceSpawn();
		spawn_time_ = spawn_cool_time_;
		spawn_count_--;
	}
}

void SpawnerComponent::SetSpawnerInfo(int spawn_count, float spawn_time, float spawn_cool_time)
{
	spawn_count_ = spawn_count;
	spawn_time_ = spawn_time;
	spawn_cool_time_ = spawn_cool_time;
}

void SpawnerComponent::ActivateSpawn()
{
	is_spawn_ = true;
}

void SpawnerComponent::AddComponent(std::unique_ptr<Component> component)
{
	component_list_.emplace_back(std::move(component));
}

void SpawnerComponent::AddComponent(Component* component)
{
	component_list_.emplace_back();
	component_list_.back().reset(component);
}

void SpawnerComponent::ForceSpawn()
{
	Object* new_object{ nullptr };
	if (model_info_)
	{
		new_object = model_info_->GetInstance();
	}
	else
	{
		new_object = new Object();
	}
	for (const std::unique_ptr<Component>& component : component_list_)
	{
		Component* new_component = component->GetCopy();
		new_object->AddComponent(new_component);
		new_component->set_owner(new_object);
	}
	new_object->set_position_vector(owner_->world_position_vector());
	scene_->AddObject(new_object);
}
