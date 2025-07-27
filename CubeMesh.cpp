#include "stdafx.h"
#include "CubeMesh.h"
#include "Shader.h"
#include "Material.h"

CubeMesh::CubeMesh()
{
	shader_type_ = (int)ShaderType::kStandardMesh;

    positions_.reserve(24);
    normals_.reserve(24);
    uvs_.resize(24);
    tangents_.resize(24);
    indices_array_.resize(1); 

    // Front face (+Z)
    positions_.emplace_back(-1, 1, -1);  // top left
    positions_.emplace_back(1, 1, -1);   // top right
    positions_.emplace_back(-1, -1, -1); // bottom left
    positions_.emplace_back(1, -1, -1);  // bottom right

    for (int i = 0; i < 4; ++i)
        normals_.emplace_back(0, 0, -1);

    // Back face (-Z)
    positions_.emplace_back(1, 1, 1);    // top right
    positions_.emplace_back(-1, 1, 1);   // top left
    positions_.emplace_back(1, -1, 1);   // bottom right
    positions_.emplace_back(-1, -1, 1);  // bottom left

    for (int i = 0; i < 4; ++i)
        normals_.emplace_back(0, 0, 1);

    // Left face (-X)
    positions_.emplace_back(-1, 1, 1);   // top front
    positions_.emplace_back(-1, 1, -1);  // top back
    positions_.emplace_back(-1, -1, 1);  // bottom front
    positions_.emplace_back(-1, -1, -1); // bottom back

    for (int i = 0; i < 4; ++i)
        normals_.emplace_back(-1, 0, 0);

    // Right face (+X)
    positions_.emplace_back(1, 1, -1);   // top back
    positions_.emplace_back(1, 1, 1);    // top front
    positions_.emplace_back(1, -1, -1);  // bottom back
    positions_.emplace_back(1, -1, 1);   // bottom front

    for (int i = 0; i < 4; ++i)
        normals_.emplace_back(1, 0, 0);

    // Top face (+Y)
    positions_.emplace_back(-1, 1, 1);   // front left
    positions_.emplace_back(1, 1, 1);    // front right
    positions_.emplace_back(-1, 1, -1);  // back left
    positions_.emplace_back(1, 1, -1);   // back right

    for (int i = 0; i < 4; ++i)
        normals_.emplace_back(0, 1, 0);

    // Bottom face (-Y)
    positions_.emplace_back(-1, -1, -1); // back left
    positions_.emplace_back(1, -1, -1);  // back right
    positions_.emplace_back(-1, -1, 1);  // front left
    positions_.emplace_back(1, -1, 1);   // front right

    for (int i = 0; i < 4; ++i)
        normals_.emplace_back(0, -1, 0);

    auto& index_buffer = indices_array_.back();
    index_buffer.reserve(36);

    // 각 면마다 인덱스 6개 (2 triangles per quad)

    // front
    index_buffer.insert(index_buffer.end(), { 0, 1, 3, 0, 3, 2 });
    // back
    index_buffer.insert(index_buffer.end(), { 4, 5, 7, 4, 7, 6 });
    // left
    index_buffer.insert(index_buffer.end(), { 8, 9,11, 8,11,10 });
    // right
    index_buffer.insert(index_buffer.end(), { 12,13,15,12,15,14 });
    // top
    index_buffer.insert(index_buffer.end(), { 16,17,19,16,19,18 });
    // bottom
    index_buffer.insert(index_buffer.end(), { 20,21,23,20,23,22 });
}
