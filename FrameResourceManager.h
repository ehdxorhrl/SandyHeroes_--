#pragma once

#include "FrameResource.h"

//다중 프레임을 위한 프레임리소스를 관리하는 클래스
//싱글톤으로 사용한다.
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

	// 프레임리소스들을 초기화한다.
	void ResetFrameResources(ID3D12Device* device, UINT cb_pass_count,
		UINT cb_object_count, UINT cb_skinned_mesh_object_count,
		UINT cb_material_count, UINT cb_ui_mesh_count);

	//프레임리소드들을 순환 시킨다 (즉, 다음 프레임리소스에 대한 업데이트 시작)
	void CirculateFrameResource(ID3D12Fence* fence);

	//다중프레임 개수
	static const int kFrameCount = 3;

private:
	static FrameResourceManager* kFrameResourceManager;

	//다중프레임을 위한 리소스들을 담은 벡터
	std::vector<std::unique_ptr<FrameResource>> frame_resources_;

	//현재 프레임 리소스
	FrameResource* curr_frame_resource_ = nullptr;

	//현재 프레임리소스의 인덱스
	int curr_frame_resource_index_ = 0;

	int pass_count_ = 0;
	int object_count_ = 0;
	int skinned_mesh_object_count_ = 0;
	int material_count_ = 0;

};