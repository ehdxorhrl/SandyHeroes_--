#pragma once
#include "Component.h"
#include "Material.h"
#include "Mesh.h"

class Mesh;
class Material;

class MeshComponent 
    : public Component 
{
public:
    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(Object* owner, Mesh* mesh, Material* material);
    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& rhs);

    virtual ~MeshComponent();

    virtual Component* GetCopy() override;

    Mesh* GetMesh() const;

    void set_mesh(Mesh* mesh);

    void AddMaterial(Material* material);

    bool ChangeMaterial(int index, Material* material);

    bool IsVisible() const;

    void set_is_visible(bool value);

protected:
    Mesh* mesh_ = nullptr;

    std::vector<Material*> materials_;

    bool is_visible_ = true;

    bool is_in_view_frustum_ = false;
};