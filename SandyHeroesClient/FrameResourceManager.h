#pragma once

#include "FrameResource.h"

//���� �������� ���� �����Ӹ��ҽ��� �����ϴ� Ŭ����
//�̱������� ����Ѵ�.
class FrameResourceManager
{
private:
	FrameResourceManager(const FrameResourceManager& other) = delete;
	FrameResourceManager& operator=(const FrameResourceManager& rhs) = delete;

public:
	FrameResourceManager();
	~FrameResourceManager() {}

	//getter
	FrameResource* curr_frame_resource() const;
	int curr_frame_resource_index() const;
	int pass_count() const;
	int object_count() const;
	int skinned_mesh_object_count() const;

	FrameResource* GetResource(int index) const;

	void ResetFrameResources(ID3D12Device* device, UINT cb_pass_count, 
		UINT cb_object_count, UINT cb_skinned_mesh_object_count, 
		UINT cb_material_count, UINT cb_ui_mesh_count);

	void CirculateFrameResource(ID3D12Fence* fence);

	//프레임리소스 개수
	static const int kFrameCount = 3;

private:
	static FrameResourceManager* kFrameResourceManager;

	std::vector<std::unique_ptr<FrameResource>> frame_resources_;

	FrameResource* curr_frame_resource_ = nullptr;

	int curr_frame_resource_index_ = 0;

	int pass_count_ = 0;
	int object_count_ = 0;
	int skinned_mesh_object_count_ = 0;
	int material_count_ = 0;

};

