#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Object.h"
#include "Material.h"
#include "Scene.h"

MeshComponent::MeshComponent(Object* owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
    mesh->AddMeshComponent(this);

    //if (!mesh) {
    //    OutputDebugStringA("MeshComponent 생성 실패: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 생성 성공: " + mesh->name() + "\n").c_str());
    //}
}
MeshComponent::MeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
    mesh->AddMeshComponent(this);

    //if (!mesh) {
    //    OutputDebugStringA("MeshComponent 생성 실패: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 생성 성공: " + mesh->name() + "\n").c_str());
    //}
}

MeshComponent::MeshComponent(Object* owner, Mesh* mesh, Material* material) : Component(owner), mesh_(mesh)
{
    mesh->AddMeshComponent(this);
    if(material)
        AddMaterial(material);

    //if (!mesh) {
    //    OutputDebugStringA("MeshComponent 생성 실패: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 생성 성공: " + mesh->name() + "\n").c_str());
    //}
}
MeshComponent::MeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh, Material* material) : Component(owner), mesh_(mesh)
{
    mesh->AddMeshComponent(this);
    if(material)
        AddMaterial(material);

    //if (!mesh) {
    //    OutputDebugStringA("MeshComponent 생성 실패: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 생성 성공: " + mesh->name() + "\n").c_str());
    //}
}

MeshComponent::MeshComponent(const MeshComponent& other) : Component(other), mesh_(other.mesh_)
{
    other.mesh_->AddMeshComponent(this);
    materials_.reserve(other.materials_.size());
    for (const auto& const material : other.materials_)
    {
        AddMaterial(material);
    }

    //if (!mesh_) {
    //    OutputDebugStringA("MeshComponent 생성 실패: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 생성 성공: " + mesh_->name() + "\n").c_str());
    //}
}

MeshComponent& MeshComponent::operator=(const MeshComponent& rhs)
{
    mesh_ = rhs.mesh_;
    mesh_->AddMeshComponent(this);
    return *this;
}

MeshComponent::~MeshComponent()
{
    if (mesh_)
    {
        mesh_->DeleteMeshComponent(this);
    }
}

void MeshComponent::set_mesh(Mesh* mesh) {
    mesh_ = mesh;
}

Component* MeshComponent::GetCopy()
{
    return new MeshComponent(*this);
}

Mesh* MeshComponent::GetMesh() const
{
    return mesh_;
}


void MeshComponent::AddMaterial(Material* material)
{
    materials_.push_back(material);
    material->AddMeshComponent(this);
}

bool MeshComponent::ChangeMaterial(int index, Material* material)
{
    if (materials_.size() < index)
    {
        return false;
    }

    materials_[index] = material;
    material->AddMeshComponent(this);

    return true;
}

bool MeshComponent::IsVisible() const
{
    return is_visible_;
}

void MeshComponent::set_is_visible(bool value)
{
    is_visible_ = value;
}
