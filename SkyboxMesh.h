#pragma once
#include "Mesh.h"

class SkyboxMesh :
    public Mesh
{
public:
    SkyboxMesh();
    SkyboxMesh(Mesh* source_mesh, Material* material);

    static Material* CreateSkyboxMaterial(const std::string& texture_name);

};

