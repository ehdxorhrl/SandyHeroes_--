#include "stdafx.h"
#include "MeshColliderComponent.h"
#include "Component.h"
#include "Object.h"
#include "Mesh.h"
#include "XMathUtil.h"


MeshColliderComponent::MeshColliderComponent(Object* owner) : Component(owner)
{
}

MeshColliderComponent::MeshColliderComponent(const MeshColliderComponent& other) : Component(other.owner_)
{
	mesh_ = other.mesh_;
}

Component* MeshColliderComponent::GetCopy()
{
	return new MeshColliderComponent(*this);
}

bool MeshColliderComponent::CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)
{
	XMFLOAT4X4 mat = owner_->world_matrix();
	XMMATRIX world = XMLoadFloat4x4(&mat);
	XMVECTOR det = XMMatrixDeterminant(world);
	XMMATRIX inverse_world = XMMatrixInverse(&det, world);

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

		//TODO: Ʈ���̾ޱ� ��Ʈ���� ���� ó�� ����
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
	XMFLOAT4X4 world_matrix_copy = owner_->world_matrix(); 
	XMMATRIX world = XMLoadFloat4x4(&world_matrix_copy);

	bool is_collide{ false };
	if (mesh_->bounds().Intersects(obb))
	{
		auto& positions = mesh_->positions();
		auto& indices_array = mesh_->indices_array();

		//TODO: Ʈ���̾ޱ� ��Ʈ���� ���� ó�� ����
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

	// ���� ��ǥ��� ��ȯ
	XMFLOAT4X4 world_mat = owner_->world_matrix();      
	XMMATRIX world = XMLoadFloat4x4(&world_mat);        
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
