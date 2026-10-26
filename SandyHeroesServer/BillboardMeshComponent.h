#pragma once
#include "MeshComponent.h"

class Scene;

class BillboardMeshComponent :
    public MeshComponent
{
public:
    DECLARE_COMPONENT(BillboardMeshComponent, MeshComponent)

    BillboardMeshComponent(Object* owner, Mesh* mesh);
    BillboardMeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh);
    BillboardMeshComponent(Object* owner, Mesh* mesh, Material* material);
    BillboardMeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh, Material* material);
    BillboardMeshComponent(Object* owner, Mesh* mesh, Material* material, Scene* scene);
    BillboardMeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh, Material* material, Scene* scene);

    BillboardMeshComponent(const BillboardMeshComponent& other);

    virtual ~BillboardMeshComponent() {}

    virtual Component* GetCopy() override;


private:
    Scene* scene_ = nullptr;

};

