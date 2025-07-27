#include "stdafx.h"
#include "CameraComponent.h"
#include "Object.h"
#include "MeshComponent.h"

CameraComponent::CameraComponent(Object* owner, 
	float near_plane_distance, float far_plane_distance, float aspect_ratio, float fov_angle) : Component(owner)
{
	CreateProjectionMatrix(near_plane_distance, far_plane_distance, aspect_ratio, fov_angle);
	BoundingFrustum::CreateFromMatrix(view_frustum_, XMLoadFloat4x4(&projection_matrix_));
}

CameraComponent::CameraComponent(const CameraComponent& other) : 
	Component(nullptr), 
	view_matrix_(other.view_matrix_), projection_matrix_(other.projection_matrix_)
{

}

Component* CameraComponent::GetCopy()
{
	return new CameraComponent(*this);
}

void CameraComponent::Update(float elapsed_time)
{
	auto view = XMLoadFloat4x4(&view_matrix_);
	auto inv_view = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	view_frustum_.Transform(world_frustum_, inv_view);
}

void CameraComponent::CreateProjectionMatrix(
	float near_plane_distance, float far_plane_distance, float aspect_ratio, float fov_angle)
{
	projection_matrix_ = 
		xmath_util_float4x4::PerspectiveFovLH(
			XMConvertToRadians(fov_angle), aspect_ratio, near_plane_distance, far_plane_distance);

}

bool CameraComponent::CollisionCheckByMeshComponent(MeshComponent* mesh_component)
{
	BoundingOrientedBox obb;
	auto aabb = mesh_component->GetMesh()->bounds();
	BoundingOrientedBox::CreateFromBoundingBox(obb, aabb);
	obb.Transform(obb, XMLoadFloat4x4(& mesh_component->owner()->world_matrix()));
	return world_frustum_.Intersects(obb);
}

using namespace xmath_util_float3;
void CameraComponent::UpdateCameraInfo()
{
	XMFLOAT3 world_up_vector{ 0,1,0 };
	XMFLOAT3 look_vector = Normalize(owner_->world_look_vector());
	XMFLOAT3 right_vector = Normalize(CrossProduct(world_up_vector, owner_->world_look_vector()));
	XMFLOAT3 up_vector = CrossProduct(look_vector, right_vector);
	XMFLOAT3 position = owner_->world_position_vector();
	world_position_ = position;
	up_vector_ = up_vector;

	view_matrix_._11 = right_vector.x; view_matrix_._12 = up_vector.x; view_matrix_._13 = look_vector.x;
	view_matrix_._21 = right_vector.y; view_matrix_._22 = up_vector.y; view_matrix_._23 = look_vector.y;
	view_matrix_._31 = right_vector.z; view_matrix_._32 = up_vector.z; view_matrix_._33 = look_vector.z;
	view_matrix_._41 = -DotProduct(position, right_vector);
	view_matrix_._42 = -DotProduct(position, up_vector);
	view_matrix_._43 = -DotProduct(position, look_vector);
}

XMFLOAT4X4 CameraComponent::view_matrix() const
{
	return view_matrix_;
}

XMFLOAT4X4 CameraComponent::projection_matrix() const
{
	return projection_matrix_;
}

XMFLOAT3 CameraComponent::world_position() const
{
	return world_position_;
}

XMFLOAT3 CameraComponent::up_vector() const
{
	return up_vector_;
}

BoundingFrustum CameraComponent::view_frustum() const
{
	return view_frustum_;
}
