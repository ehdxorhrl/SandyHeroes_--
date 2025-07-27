#pragma once

#include <vector>
#include <string>
#include <list>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;

class MeshComponent;
//class FrameResourceManager;
//struct FrameResource;
//class DescriptorManager;
//class Material;

// 메쉬 정보를 가지는 클래스
class Mesh {
public:
    Mesh() {}
    virtual ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void AddMeshComponent(MeshComponent* mesh_component);
    void DeleteMeshComponent(MeshComponent* mesh_component);

    void LoadMeshFromFile(std::ifstream& file);

    const std::list<MeshComponent*>& mesh_component_list() const;

    const std::vector<XMFLOAT3>& positions() const;
    const std::vector<std::vector<UINT>>& indices_array() const;

    BoundingBox bounds() const;
    
    std::string name() const;
    void set_name(const std::string& name);

    D3D_PRIMITIVE_TOPOLOGY primitive_topology() const;

    void ClearNormals()
    {
        normals_.clear();
    }

    void ClearTangents()
    {
        tangents_.clear();
    }

protected:
    D3D_PRIMITIVE_TOPOLOGY primitive_topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    std::vector<XMFLOAT3> positions_;

    std::vector<XMFLOAT4> colors_;

    std::vector<XMFLOAT2> uvs_;

    std::vector<XMFLOAT3> normals_;

    std::vector<XMFLOAT3> tangents_;

    std::vector<XMFLOAT3> bi_tangents_;

    std::vector<std::vector<UINT>> indices_array_;

    BoundingBox bounds_{};
    std::string name_ = "None";

    std::list<MeshComponent*> mesh_component_list_;
};

