#include "stdafx.h"
#include "DebugMeshComponent.h"
#include "Object.h"
#include "Mesh.h"


DebugMeshComponent::DebugMeshComponent(Object* owner, Mesh* cube_mesh, Mesh* source_mesh)
	: MeshComponent(owner, cube_mesh), bounds_(source_mesh->bounds())
{
}

DebugMeshComponent::DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingBox& bounds)
	: MeshComponent(owner, cube_mesh), bounds_(bounds)
{
}

DebugMeshComponent::DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingOrientedBox& bounds)
	: MeshComponent(owner, cube_mesh)
{
	bounds_.Center = bounds.Center;
	bounds_.Extents = bounds.Extents;
}

DebugMeshComponent::DebugMeshComponent(const DebugMeshComponent& other)
	: MeshComponent(other), bounds_(other.bounds_)
{
}

Component* DebugMeshComponent::GetCopy()
{
	return new DebugMeshComponent(*this);
}



