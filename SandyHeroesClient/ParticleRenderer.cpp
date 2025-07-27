#include "stdafx.h"
#include "ParticleRenderer.h"
#include "DescriptorManager.h"
#include "ParticleComponent.h"
#include "CameraComponent.h"
#include "FrameResource.h"

void ParticleRenderer::ParticleRender(
	ID3D12GraphicsCommandList* command_list, 
	FrameResource* curr_frame_resource,
	DescriptorManager* descriptor_manager,
	CameraComponent* camera)
{
	for (ParticleComponent* particleComponent : particle_components_)
	{
		particleComponent->material()->Render(command_list, curr_frame_resource, descriptor_manager, camera);
		particleComponent->Render(command_list, curr_frame_resource);
	}

	particle_components_.clear();
}
