#include "stdafx.h"
#include "MeshColliderComponent.h"
#include "Object.h"
#include "Mesh.h"

MeshColliderComponent::MeshColliderComponent(Object* owner) : Component(owner)
{
}
MeshColliderComponent::MeshColliderComponent(const std::shared_ptr<Object>& owner) : Component(owner)
{
}

MeshColliderComponent::MeshColliderComponent(const MeshColliderComponent& other) : Component(other)
{
	mesh_ = other.mesh_;
}

Component* MeshColliderComponent::GetCopy()
{
	return new MeshColliderComponent(*this);
}

bool MeshColliderComponent::CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)
{
	auto locked_owner = owner_.lock();
	if (!locked_owner) return false;
	XMMATRIX world = XMLoadFloat4x4(&locked_owner->world_matrix());
	XMMATRIX inverse_world = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	BoundingOrientedBox bounds;
	BoundingOrientedBox::CreateFromBoundingBox(bounds, mesh_->bounds());
	bounds.Transform(bounds, world);

	float t{};
	float t_min{ std::numeric_limits<float>::max() };
	bool is_collide{ false };
	if (bounds.Intersects(ray_origin, ray_direction, t))
	{
		std::string name = mesh()->name();
		//ö���� 1���˻縸 ����
		if ("Fence_01" == name ||
			"Fence_02" == name ||
			"Fence_03" == name || 
			"Fence_04" == name || 
			"Fence_05" == name)
		{
			out_distance = t;
			is_collide = true;
			return is_collide;
		}
			

		auto& positions = mesh_->positions();
		auto& indices_array = mesh_->indices_array();

		if (mesh_->primitive_topology() == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			for (auto& indices : indices_array)
			{
				for (int i = 0; i < indices.size(); i += 3)
				{
					UINT i0 = indices[i + 0];
					UINT i1 = indices[i + 1];
					UINT i2 = indices[i + 2];

					XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[i0]), world);
					XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[i1]), world);
					XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[i2]), world);

					if (TriangleTests::Intersects(ray_origin, ray_direction, v0, v1, v2, t))
					{
						is_collide = true;
						if (t < t_min) // ���� �������� ����� �ﰢ������ ���� �Ű�����
						{
							t_min = t;
						}
					}
				}
			}
		}
	}
	if (is_collide)
	{
		out_distance = t_min;
	}

	return is_collide;
}

bool MeshColliderComponent::CollisionCheckByObb(BoundingOrientedBox obb)
{
	auto locked_owner = owner_.lock();
	if (!locked_owner) return false;
	XMMATRIX world = XMLoadFloat4x4(&locked_owner->world_matrix());

	bool is_collide{ false };
	if (mesh_->bounds().Intersects(obb))
	{
		auto& positions = mesh_->positions();
		auto& indices_array = mesh_->indices_array();

		if (mesh_->primitive_topology() == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			for (auto& indices : indices_array)
			{
				for (int i = 0; i < indices.size(); i += 3)
				{
					UINT i0 = indices[i + 0];
					UINT i1 = indices[i + 1];
					UINT i2 = indices[i + 2];

					XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[i0]), world);
					XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[i1]), world);
					XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[i2]), world);

					if (obb.Intersects(v0, v1, v2))
					{
						is_collide = true;
					}
				}
			}
		}
	}

	return is_collide;
}

BoundingOrientedBox MeshColliderComponent::GetWorldOBB() const
{
	if (!mesh_) return BoundingOrientedBox{};

	BoundingOrientedBox obb;
	BoundingOrientedBox::CreateFromBoundingBox(obb, mesh_->bounds());

	//  ǥ ȯ
	auto locked_owner = owner_.lock();
	if (!locked_owner) return BoundingOrientedBox{};
	XMMATRIX world = XMLoadFloat4x4(&locked_owner->world_matrix());
	obb.Transform(obb, world);

	return obb;
}

Mesh* MeshColliderComponent::mesh() const
{
	return mesh_;
}


void MeshColliderComponent::set_mesh(Mesh* mesh)
{
	mesh_ = mesh;
}
