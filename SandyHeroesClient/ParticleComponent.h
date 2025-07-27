#pragma once
#include "stdafx.h"
#include "Component.h"
#include "UploadBuffer.h"
#include "Material.h"
#include "Object.h"
#include "Scene.h"

struct FrameResource;

class ParticleComponent :
	public Component
{
private:
	struct ParticleData {
		XMVECTOR velocity_;
		float speed_;
		float life_time_;
		float max_life_time_;
		
		bool IsActive() const { return life_time_ > 0.0f; }
	};
public:
	struct Particle {
		XMFLOAT4 color_;
		XMFLOAT3 position_;
		//float life_factor_;
		XMFLOAT2 size_;
	};

	enum eShape {
		Sphere,
		Cone,
		Circle,
		BigCone,
		Count
	};

	ParticleComponent(Object* owner);
	ParticleComponent(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material);

	ParticleComponent(const ParticleComponent& other);

	virtual Component* GetCopy() override;

	void Initialize(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material);

	virtual void Update(float elapsed_time) override;
	virtual void Render(ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

	bool IsAlive(const int index) const;
	Particle particle(const int index) const;
	UINT capacity() const;
	Material* material() const;

	//getter
	XMFLOAT4 color() const;

	//setter
	void set_scene(Scene* value);
	void set_hit_position(XMFLOAT3 value);
	void set_loop(bool value);
	void set_direction_pivot_object(Object* value);
	void set_color(const XMFLOAT4& color);

	void Play(int particle_count);

	static ParticleRenderer* kParticleRenderer;

private:
	Scene* scene_ = nullptr;

	Object* direction_pivot_object_ = nullptr;

	ID3D12Device* device_ = nullptr;

	UINT capacity_ = 50;
	UINT alive_count_ = 0;
	std::vector<Particle> particles_ = {};
	std::vector<ParticleData> particle_data_ = {};

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_ = {};
	std::unique_ptr<UploadBuffer<Particle>> particle_buffer_ = nullptr;

	Material* material_ = nullptr;

	eShape shape_ = eShape::Sphere;

	XMFLOAT3 hit_position_ = {};
	bool loop_ {false};
	XMFLOAT4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};