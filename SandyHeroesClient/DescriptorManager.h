#pragma once

// 디스크립터를 관리하는 클래스
// 현재는 CBV만 있지만 향후 텍스처를 추가하는 과정에서 SRV에 관한 내용이 추가될 예정
class DescriptorManager
{
private:
	DescriptorManager(const DescriptorManager& other) = delete;
	DescriptorManager& operator=(const DescriptorManager& rhs) = delete;

public:
	DescriptorManager();
	~DescriptorManager() {}


	//getter
	UINT cbv_bone_transform_offset() const;
	UINT cbv_pass_offset() const;
	UINT srv_offset() const;
	UINT texture_count() const;

	void ResetDescriptorHeap(ID3D12Device* device, int texture_count);

	ID3D12DescriptorHeap* GetDescriptorHeap() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(int index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(int index) const;

	static UINT kCbvSrvUavDescriptorSize;

private:
	static DescriptorManager* kDescriptorManager;

	ComPtr<ID3D12DescriptorHeap> d3d_cbv_srv_uav_heap_ = nullptr;

	// 각 상수버퍼뷰의 오프셋
	UINT cbv_bone_transform_offset_ = 0;
	UINT cbv_pass_offset_ = 0; 

	UINT srv_offset_ = 0;
	
	UINT texture_count_ = 0;
};

