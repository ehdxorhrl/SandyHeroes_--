#pragma once
#include "Mesh.h"

struct Texture;

class SkyboxMesh :
    public Mesh
{
public:
    SkyboxMesh();
    SkyboxMesh(Mesh* source_mesh);

    static Material* CreateSkyboxMaterial(const std::string& texture_name, Texture* skybox_texture);

};

