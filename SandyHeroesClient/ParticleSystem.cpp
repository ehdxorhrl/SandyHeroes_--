#include "stdafx.h"
#include "ParticleSystem.h"
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"
#include "MovementComponent.h"
#include "MeshComponent.h"

ParticleSystem::ParticleSystem(Mesh* particle_mesh, Material* particle_material) 
	: particle_mesh_(particle_mesh), particle_material_(particle_material)
{
}

Object* ParticleSystem::CreateParticle(XMFLOAT3 position, XMFLOAT3 direction, float speed)
{
	Object* particle = new Object();
	particle->Scale(0.06f);
	particle->set_position_vector(position);
	particle->AddComponent(new MeshComponent(particle, particle_mesh_, particle_material_));
	MovementComponent* movement_component = new MovementComponent(particle);
	movement_component->DisableFriction();
	movement_component->DisableGarvity();
	movement_component->Move(direction, speed);
	movement_component->set_max_speed(speed);
	movement_component->set_max_speed_xz(speed);
	particle->AddComponent(movement_component);

	constexpr float kMinLifeTime = 0.2f;
	constexpr float kMaxLifeTime = 0.7f;
	std::uniform_real_distribution<float> dist(kMinLifeTime, kMaxLifeTime);
	float life_time{ dist(kRandomGenerator) };
	particle_list_.emplace_back(particle, life_time);

	return particle;
}

void ParticleSystem::SpawnParticle(Scene* scene, XMFLOAT3 position, int particle_count, float particle_time)
{
	constexpr float kMinSpeed = 1.f;
	constexpr float kMaxSpeed = 4.f;
	XMVECTOR direction = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX rotation_z = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f));
	XMMATRIX rotation_y = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(30.f));
	XMFLOAT3 direction_f3{};
	XMStoreFloat3(&direction_f3, direction);

	std::uniform_real_distribution<float> dist(kMinSpeed, kMaxSpeed);
	float particle_speed{ dist(kRandomGenerator) };

	scene->AddObject(CreateParticle(position, direction_f3, particle_speed));
	for (int i = 0; i < 5; ++i)
	{
		// z축을 기준으로 기준축 회전
		direction = XMVector3Transform(direction, rotation_z);
		XMStoreFloat3(&direction_f3, direction);
		for (int j = 0; j < 12; ++j)
		{
			// 파티클 생성후 y축을 기준으로 회전
			particle_speed = dist(kRandomGenerator);
			scene->AddObject(CreateParticle(position, direction_f3, particle_speed));
			direction = XMVector3Transform(direction, rotation_y);
			XMStoreFloat3(&direction_f3, direction);
		}
	}
	direction = XMVector3Transform(direction, rotation_z);
	XMStoreFloat3(&direction_f3, direction);
	particle_speed = dist(kRandomGenerator);
	scene->AddObject(CreateParticle(position, direction_f3, particle_speed));

}

void ParticleSystem::Update(float elapsed_time)
{
	for (auto& particle : particle_list_)
	{
		particle.life_time -= elapsed_time;
		if (particle.life_time <= 0.f)
		{
			particle.particle_object->set_is_dead(true);
		}
	}
	particle_list_.remove_if([](Particle& particle) {
		if (particle.particle_object->is_dead())
		{
			return true;
		}
		return false;
		});
}
