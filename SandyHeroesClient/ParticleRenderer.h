#pragma once

struct FrameResource;
class DescriptorManager;
class ParticleComponent;
class CameraComponent;

class ParticleRenderer
{
public:
	ParticleRenderer() = default;
	~ParticleRenderer() = default;

	void AddParticleComponent(ParticleComponent* particle_component)
	{
		particle_components_.push_back(particle_component);
	}

	void ParticleRender(
		ID3D12GraphicsCommandList* command_list,
		FrameResource* curr_frame_resource,
		DescriptorManager* descriptor_manager,
		CameraComponent* camera);

private:
	std::list<ParticleComponent*> particle_components_;
};

