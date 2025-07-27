#include "stdafx.h"
#include "DescriptorManager.h"

UINT DescriptorManager::kCbvSrvUavDescriptorSize = 0;
DescriptorManager* DescriptorManager::kDescriptorManager = nullptr;

DescriptorManager::DescriptorManager()
{
	assert(!kDescriptorManager);
	kDescriptorManager = this;
}

UINT DescriptorManager::cbv_bone_transform_offset() const
{
	return cbv_bone_transform_offset_;
}

UINT DescriptorManager::cbv_pass_offset() const
{
	return cbv_pass_offset_;
}

UINT DescriptorManager::srv_offset() const
{
	return srv_offset_;
}

UINT DescriptorManager::texture_count() const
{
	return texture_count_;
}

void DescriptorManager::ResetDescriptorHeap(ID3D12Device* device, int texture_count)
{
	d3d_cbv_srv_uav_heap_.Reset();

	//25.02.25 수정
	// 상수 버퍼를 루트 테이블이 아니라 루트 CBV로 사용하면서 아래코드를 사용하지 않음
	//// 각 프레임 리소스마다 각 오브젝트의 CBV가 필요 + 랜더패스 1개
	//UINT num_descriptors = (object_count + skinned_mesh_object_count + 1) * frame_resource_count;

	//cbv_bone_transform_offset_ = object_count * frame_resource_count;
	//cbv_pass_offset_ = (object_count + skinned_mesh_object_count) * frame_resource_count;

	//D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc{};
	//cbv_heap_desc.NumDescriptors = num_descriptors;
	//cbv_heap_desc.NodeMask = 0;
	//cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
	heap_desc.NumDescriptors = texture_count;
	heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heap_desc.NodeMask = 0;

	texture_count_ = texture_count;
	device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(d3d_cbv_srv_uav_heap_.GetAddressOf()));

}

ID3D12DescriptorHeap* DescriptorManager::GetDescriptorHeap() const
{
	return d3d_cbv_srv_uav_heap_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorManager::GetCpuHandle(int index) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE r_value =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(d3d_cbv_srv_uav_heap_->GetCPUDescriptorHandleForHeapStart());
	r_value.Offset(index, kCbvSrvUavDescriptorSize);
	return r_value;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorManager::GetGpuHandle(int index) const
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE r_value = 
		CD3DX12_GPU_DESCRIPTOR_HANDLE(d3d_cbv_srv_uav_heap_->GetGPUDescriptorHandleForHeapStart());
	r_value.Offset(index, kCbvSrvUavDescriptorSize);
	return r_value;
}
