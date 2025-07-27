#pragma once

class MeshComponent;
class FrameResourceManager;
struct FrameResource;
class DescriptorManager;
class Material;

// 메쉬 정보를 가지는 클래스
class Mesh
{
public:
	Mesh() {}
	virtual ~Mesh() {}
	// 메쉬 정보는 복사될 이유가 없음
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void AddMeshComponent(MeshComponent* mesh_component);
	void DeleteMeshComponent(MeshComponent* mesh_component);

	void AddMaterial(Material* material);

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);

	virtual void ReleaseUploadBuffer();

	//이 메쉬를 사용하는 object의 개수만큼 cb를 업데이트
	virtual void UpdateConstantBuffer(FrameResource* curr_frame_resource);

	virtual void Render(ID3D12GraphicsCommandList* command_list,
		FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager);

	void LoadMeshFromFile(std::ifstream& file);

	//정점 쉐이터 입력 레이아웃을 맞추기 위해 정점의 일부 정보을 제거하는 함수들
	void ClearColors();
	void ClearUvs();
	void ClearNormals();
	void ClearTangents();

	//getter
	int shader_type() const;
	std::string name() const;

	const std::list<MeshComponent*>& mesh_component_list() const;

	BoundingBox bounds() const;
	const std::vector<XMFLOAT3>& positions() const;
	const std::vector<std::vector<UINT>>& indices_array() const;
	D3D12_PRIMITIVE_TOPOLOGY primitive_topology() const;

	//setter
	void set_shader_type(int value);
	void set_name(const std::string& name);

	static void ResetCBObjectCurrentIndex();

protected:
	std::vector<Material*> materials_;

	D3D12_PRIMITIVE_TOPOLOGY primitive_topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<XMFLOAT3> positions_;
	ComPtr<ID3D12Resource> d3d_position_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_position_upload_buffer_ = nullptr;

	std::vector<XMFLOAT4> colors_;
	ComPtr<ID3D12Resource> d3d_color_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_color_upload_buffer_ = nullptr;

	std::vector<XMFLOAT2> uvs_;
	ComPtr<ID3D12Resource> d3d_uv_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_uv_upload_buffer_ = nullptr;

	std::vector<XMFLOAT3> normals_;
	ComPtr<ID3D12Resource> d3d_normal_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_normal_upload_buffer_ = nullptr;

	std::vector<XMFLOAT3> tangents_;
	ComPtr<ID3D12Resource> d3d_tangent_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_tangent_upload_buffer_ = nullptr;

	std::vector<XMFLOAT3> bi_tangents_;
	ComPtr<ID3D12Resource> d3d_bi_tangent_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_bi_tangent_upload_buffer_ = nullptr;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views_;

	//대부분의 메쉬는 1개의 인덱스 버퍼를 사용하지만 
	//일부 메쉬의 경우 정점을 공유한채로 인덱스 버퍼가 여러개인 경우가 있음
	std::vector<std::vector<UINT>> indices_array_;					
	std::vector<ComPtr<ID3D12Resource>> d3d_index_buffers_;
	std::vector<ComPtr<ID3D12Resource>> d3d_index_upload_buffers_;
	std::vector<D3D12_INDEX_BUFFER_VIEW> index_buffer_views_;

	std::string name_ = "None";

	BoundingBox bounds_{};

	// 이 메쉬를 참조중인 컴포넌트 리스트
	std::list<MeshComponent*> mesh_component_list_;

	// 메쉬가 사용하는 쉐이더의 타입 Shader.h 참고
	int shader_type_ = 0;

private:
	//업데이트 되어야 할 cb의 인덱스를 갖는 변수 Scene 렌더시 0으로 초기화 해야한다. (Reset함수로)
	static int kCBObjectCurrentIndex;
};

