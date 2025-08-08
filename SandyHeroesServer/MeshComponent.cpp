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
    //    OutputDebugStringA("MeshComponent 积己 角菩: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 积己 己傍: " + mesh->name() + "\n").c_str());
    //}
}

MeshComponent::MeshComponent(Object* owner, Mesh* mesh, Material* material) : Component(owner), mesh_(mesh)
{
    mesh->AddMeshComponent(this);
    if(material)
        AddMaterial(material);

    //if (!mesh) {
    //    OutputDebugStringA("MeshComponent 积己 角菩: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 积己 己傍: " + mesh->name() + "\n").c_str());
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
    //    OutputDebugStringA("MeshComponent 积己 角菩: mesh == nullptr\n");
    //}
    //else {
    //    OutputDebugStringA(("MeshComponent 积己 己傍: " + mesh_->name() + "\n").c_str());
    //}
}

MeshComponent& MeshComponent::operator=(const MeshComponent& rhs)
{
    owner_ = nullptr;
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

    for (const auto& const material : materials_)
    {
        material->DeleteMeshComponent(this);
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

    materials_[index]->DeleteMeshComponent(this);
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
