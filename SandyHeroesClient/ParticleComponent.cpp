#include "stdafx.h"
#include "ParticleComponent.h"
#include "FPSControllerComponent.h"
#include "BaseScene.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "ParticleRenderer.h"

ParticleRenderer* ParticleComponent::kParticleRenderer = nullptr;

ParticleComponent::ParticleComponent(Object* owner) : Component(owner)
{
}

ParticleComponent::ParticleComponent(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material) : Component(owner)
{
	Initialize(owner, device, particle_count, shape, material);
}

ParticleComponent::ParticleComponent(const ParticleComponent& other) : Component(other), 
scene_(other.scene_),
color_(other.color_)
{
	Initialize(other.owner_, other.device_, other.capacity_, other.shape_, other.material_);
}

Component* ParticleComponent::GetCopy()
{
    return new ParticleComponent(*this);
}

void ParticleComponent::Initialize(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material)
{
	capacity_ = particle_count;
	shape_ = shape;
	material_ = material;
	particles_.reserve(capacity_);
	particle_data_.reserve(capacity_);

	device_ = device;

	particle_buffer_ = std::make_unique<UploadBuffer<Particle>>(device, particle_count, false);

	for (int i = 0; i < capacity_; ++i)
	{
		XMFLOAT4 color = { 1,1,1,1 };
		XMFLOAT3 position = {};
		XMVECTOR direction = {};

		if (shape_ == Sphere)
		{
			//position = XMFLOAT3(0.0f, 5.0f, 0.0f);
			position = hit_position_;
			direction = RANDOM::InsideUnitSphere();


			particles_.emplace_back(Particle(color, position, XMFLOAT2(0.15f, 0.15f))); // sphere 텍스쳐 사이즈
			particle_data_.emplace_back(ParticleData(direction, 2.0f, 0.1f, 0.1f));
		}
		else if (shape_ == Cone)
		{
			position = owner->world_position_vector();
			XMFLOAT3 owner_forward = owner->world_look_vector();
			XMVECTOR forward = XMLoadFloat3(&owner_forward); // 기준 방향
			constexpr float coneAngle = XMConvertToRadians(45.0f); // 콘의 반각도

			//constexpr float radius = 5.0f;
			constexpr float radius = 0.0f;
			direction = RANDOM::DirectionInCone(forward, coneAngle);
			XMVECTOR randomPos = direction * radius;
			randomPos += XMVectorSet(position.x, position.y, position.z, 0);
			XMStoreFloat3(&position, randomPos);

			particles_.emplace_back(Particle(color_, position, XMFLOAT2(0.1f, 0.1f))); // cone 텍스쳐 사이즈
			particle_data_.emplace_back(ParticleData(direction, 2.0f, 0.1f, 0.1f));	//cone
		}
		else if (shape_ == Circle)
		{
			position = owner->world_position_vector();
			XMVECTOR axis = XMVectorSet(0, 1, 0, 0); // 기준 방향
			XMStoreFloat3(&position, RANDOM::CircleEdgePoint(axis, 0.5f, &direction));

			particles_.emplace_back(Particle(color_, position, XMFLOAT2(0.25f, 0.25f))); // 텍스쳐 사이즈
			particle_data_.emplace_back(ParticleData(direction, 2.0f, 0.1f, 0.1f));
		}
		else if (shape_ == BigCone)
		{
			position = owner->world_position_vector();
			XMFLOAT3 owner_forward = owner->world_look_vector();
			XMVECTOR forward = XMLoadFloat3(&owner_forward); // 기준 방향
			//constexpr float coneAngle = XMConvertToRadians(45.0f); // 콘의 반각도
			constexpr float coneAngle = XMConvertToRadians(0.0f); // 콘의 반각도

			constexpr float radius = 0.0f;
			direction = RANDOM::DirectionInCone(forward, coneAngle);
			XMVECTOR randomPos = direction * radius;
			randomPos += XMVectorSet(position.x, position.y, position.z, 0);
			XMStoreFloat3(&position, randomPos);

			particles_.emplace_back(Particle(color_, position, XMFLOAT2(0.5f, 0.5f)));
			particle_data_.emplace_back(ParticleData(direction, 10.0f, 1.0f, 1.0f));
		}
		else
		{
			assert(false);
		}

	}

	vertex_buffer_view_.BufferLocation = particle_buffer_->Resource()->GetGPUVirtualAddress();
	vertex_buffer_view_.SizeInBytes = capacity_ * sizeof(Particle);
	vertex_buffer_view_.StrideInBytes = sizeof(Particle);
}

void ParticleComponent::Update(float elapsed_time)
{
	int particleIndex = 0;

	auto base_scene = dynamic_cast<BaseScene*>(scene_);
	if (!base_scene) return;

	//Object* player = base_scene->player();  // BaseScene의 player_
	//auto controller = Object::GetComponentInChildren<FPSControllerComponent>(player);

	for (int i = 0; i < capacity_; ++i)
	{
		//TODO: Particle Movement Update.
		XMVECTOR vPosition = XMLoadFloat3(&particles_[i].position_);
		vPosition += particle_data_[i].velocity_ * particle_data_[i].speed_ * elapsed_time;
		XMStoreFloat3(&particles_[i].position_, vPosition);

		particle_data_[i].life_time_ -= elapsed_time;
		// TODO : life_factor 설정 추가하기
		//particles_[i].life_factor_ = 0.7f + particle_data_[i].life_time_ / particle_data_[i].max_life_time_ * (1.0f - 0.7f);

		//Alive인 Particle만 담아야함.
		if (particle_data_[i].IsActive())
		{
			/*if (!(shape_ == Cone && controller->is_firekey_down()))
				continue;*/
			particle_buffer_->CopyData(particleIndex++, particles_[i]);
		}
		else
		{
			if (!loop_) continue;

			XMFLOAT3 position = {};
			XMVECTOR direction = {};

			if (shape_ == Sphere)	//몬스터 HIT 파티클
			{
				//position = XMFLOAT3(0.0f, 15.0f, 0.0f);
				position = hit_position_;
				direction = RANDOM::InsideUnitSphere();

				particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.6, 0.6f);
				particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
				particle_data_[i].velocity_ = direction;
				XMFLOAT3 pivot_position = owner_->world_position_vector();
				XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
				XMStoreFloat3(&particles_[i].position_, rPosition);
			}
			else if (shape_ == Cone)	// 총 발사 파티클
			{
				//if (!controller->is_firekey_down()) break;

				position = owner_->world_position_vector();
				XMFLOAT3 owner_forward = owner_->world_look_vector();
				XMVECTOR forward = XMLoadFloat3(&owner_forward); // 기준 방향
				if (scene_)
				{
					auto camera = scene_->main_camera();
					if (camera) {
						forward = XMLoadFloat3(&camera->owner()->world_look_vector());
					}
				}
				if (direction_pivot_object_) {
					forward = XMLoadFloat3(&direction_pivot_object_->world_look_vector());
				}
				constexpr float coneAngle = XMConvertToRadians(18.0f); // 콘의 반각도 (23 기본)
				constexpr float radius = 0.2f; // (기본 5)
				direction = RANDOM::DirectionInCone(forward, coneAngle);
				XMVECTOR randomPos = direction * radius;
				//randomPos += XMVectorSet(position.x, position.y, position.z, 0);
				XMStoreFloat3(&position, randomPos);

				particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.01f, 0.1f);	// cone
				particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
				particle_data_[i].velocity_ = direction;
				XMFLOAT3 pivot_position = owner_->world_position_vector();
				XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
				XMStoreFloat3(&particles_[i].position_, rPosition);
			}
			else if (shape_ == Circle)
			{
				position = owner_->world_position_vector();
				XMVECTOR axis = XMVectorSet(0, 1, 0, 0); // 기준 방향
				XMStoreFloat3(&position, RANDOM::CircleEdgePoint(axis, 0.5f, &direction));

				particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.1f, 0.5f); // 기본
				particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
				particle_data_[i].velocity_ = direction;
				XMFLOAT3 pivot_position = owner_->world_position_vector();
				XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
				XMStoreFloat3(&particles_[i].position_, rPosition);
			}
			else if (shape_ == BigCone)	// 총 발사 파티클
			{
				//if (!controller->is_firekey_down()) break;

				position = owner_->world_position_vector();
				XMFLOAT3 owner_forward = owner_->world_look_vector();
				XMVECTOR forward = XMLoadFloat3(&owner_forward); // 기준 방향
				if (scene_)
				{
					auto camera = scene_->main_camera();
					if (camera) {
						forward = XMLoadFloat3(&camera->owner()->world_look_vector());
					}
				}

				if (direction_pivot_object_) {
					forward = XMLoadFloat3(&direction_pivot_object_->world_look_vector());
				}
				constexpr float coneAngle = XMConvertToRadians(18.0f); // 콘의 반각도 (23 기본)
				constexpr float radius = 0.2f; // (기본 5)
				direction = RANDOM::DirectionInCone(forward, coneAngle);
				XMVECTOR randomPos = direction * radius;
				//randomPos += XMVectorSet(position.x, position.y, position.z, 0);
				XMStoreFloat3(&position, randomPos);

				particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.1f, 0.5f);	// cone
				particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
				particle_data_[i].velocity_ = direction;
				XMFLOAT3 pivot_position = owner_->world_position_vector();
				XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
				XMStoreFloat3(&particles_[i].position_, rPosition);
			}
			else
			{
				assert(false);
			}
		}
	}

	alive_count_ = particleIndex;

	if (alive_count_ > 0)
	{
		kParticleRenderer->AddParticleComponent(this);
	}
}

void ParticleComponent::Render(ID3D12GraphicsCommandList* command_list,
	FrameResource* curr_frame_resource)
{
	command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view_);
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	command_list->DrawInstanced(alive_count_, 1, 0, 0); // aliveCount는 살아있는 파티클 수
}

bool ParticleComponent::IsAlive(const int index) const
{
	return particle_data_[index].IsActive();
}

ParticleComponent::Particle ParticleComponent::particle(const int index) const
{
	return particles_[index];
}

UINT ParticleComponent::capacity() const
{
	return capacity_;
}

Material* ParticleComponent::material() const
{
	return material_;
}

XMFLOAT4 ParticleComponent::color() const
{
	return color_;
}

void ParticleComponent::set_scene(Scene* value)
{
	scene_ = value;
}

void ParticleComponent::set_hit_position(XMFLOAT3 value)
{
	hit_position_ = value;
}

void ParticleComponent::set_loop(bool value)
{
	loop_ = value;
}

void ParticleComponent::set_direction_pivot_object(Object* value)
{
	direction_pivot_object_ = value;
}

void ParticleComponent::set_color(const XMFLOAT4& color)
{
	color_ = color;
	for (int i = 0; i < capacity_; ++i)
		particles_[i].color_ = color;
}

void ParticleComponent::Play(int particle_count)
{
	for (int i = 0; i < capacity_; ++i)
	{
		if (particle_count == 0) return;
		if (particle_data_[i].IsActive()) continue;
		particle_count--;

		XMFLOAT3 position = {};
		XMVECTOR direction = {};

		if (shape_ == Sphere)
		{
			//position = XMFLOAT3(0.0f, 15.0f, 0.0f);
			position = hit_position_;
			direction = RANDOM::InsideUnitSphere();

			particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.6, 0.6f);
			particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
			particle_data_[i].velocity_ = direction;
			XMFLOAT3 pivot_position = owner_->world_position_vector();
			XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
			XMStoreFloat3(&particles_[i].position_, rPosition);
		}
		else if (shape_ == Cone)
		{
			position = owner_->world_position_vector();
			XMFLOAT3 owner_forward = owner_->world_look_vector();
			XMVECTOR forward = XMLoadFloat3(&owner_forward); // 기준 방향
			if (scene_)
			{
				auto camera = scene_->main_camera();
				if (camera) {
					forward = XMLoadFloat3(&camera->owner()->world_look_vector());
				}
			}
			if (direction_pivot_object_) {
				forward = XMLoadFloat3(&direction_pivot_object_->world_look_vector());
			}
			constexpr float coneAngle = XMConvertToRadians(18.0f); // 콘의 반각도 (23 기본)
			constexpr float radius = 0.2f; // (기본 5)
			direction = RANDOM::DirectionInCone(forward, coneAngle);
			XMVECTOR randomPos = direction * radius;
			//randomPos += XMVectorSet(position.x, position.y, position.z, 0);
			XMStoreFloat3(&position, randomPos);

			particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.01f, 0.1f);	// cone
			particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
			particle_data_[i].velocity_ = direction;
			XMFLOAT3 pivot_position = owner_->world_position_vector();
			XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
			XMStoreFloat3(&particles_[i].position_, rPosition);
		}
		else if (shape_ == Circle)
		{
			position = owner_->world_position_vector();
			XMVECTOR axis = XMVectorSet(0, 1, 0, 0); // 기준 방향
			XMStoreFloat3(&position, RANDOM::CircleEdgePoint(axis, 0.5f, &direction));

			particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.1f, 0.5f); // 기본
			particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
			particle_data_[i].velocity_ = direction;
			XMFLOAT3 pivot_position = owner_->world_position_vector();
			XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
			XMStoreFloat3(&particles_[i].position_, rPosition);
		}
		else if (shape_ == BigCone)	// 총 발사 파티클
		{
			position = owner_->world_position_vector();
			XMFLOAT3 owner_forward = owner_->world_look_vector();
			XMVECTOR forward = XMLoadFloat3(&owner_forward); // 기준 방향
			if (scene_)
			{
				auto camera = scene_->main_camera();
				if (camera) {
					forward = XMLoadFloat3(&camera->owner()->world_look_vector());
				}
			}

			if (direction_pivot_object_) {
				forward = XMLoadFloat3(&direction_pivot_object_->world_look_vector());
			}
			constexpr float coneAngle = XMConvertToRadians(18.0f); // 콘의 반각도 (23 기본)
			constexpr float radius = 0.2f; // (기본 5)
			direction = RANDOM::DirectionInCone(forward, coneAngle);
			XMVECTOR randomPos = direction * radius;
			//randomPos += XMVectorSet(position.x, position.y, position.z, 0);
			XMStoreFloat3(&position, randomPos);

			particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.1f, 0.5f);	// cone
			particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
			particle_data_[i].velocity_ = direction;
			XMFLOAT3 pivot_position = owner_->world_position_vector();
			XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);
			XMStoreFloat3(&particles_[i].position_, rPosition);
		}
		else
		{
			assert(false);
		}
	}
}
