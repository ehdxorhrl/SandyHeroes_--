#include "stdafx.h"
#include "BillboardMeshComponent.h"
#include "Object.h"
#include "Scene.h"
#include "CameraComponent.h"

BillboardMeshComponent::BillboardMeshComponent(Object* owner, Mesh* mesh) : MeshComponent(owner, mesh)
{
}

BillboardMeshComponent::BillboardMeshComponent(Object* owner, Mesh* mesh, Material* material)
	: MeshComponent(owner, mesh, material)
{
}

BillboardMeshComponent::BillboardMeshComponent(Object* owner, Mesh* mesh, Material* material, Scene* scene)
	: MeshComponent(owner, mesh, material), scene_(scene)
{
}

BillboardMeshComponent::BillboardMeshComponent(const BillboardMeshComponent& other) : MeshComponent(other), scene_(other.scene_)
{
}

Component* BillboardMeshComponent::GetCopy()
{
	return new BillboardMeshComponent(*this);
}

