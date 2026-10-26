#include "stdafx.h"
#include "FrameResourceManager.h"
#include "FrameResource.h"

FrameResourceManager* FrameResourceManager::kFrameResourceManager = nullptr;

FrameResourceManager::FrameResourceManager()
{
    assert(!kFrameResourceManager);
    kFrameResourceManager = this;
}

FrameResource* FrameResourceManager::curr_frame_resource() const
{
    return curr_frame_resource_;
}

int FrameResourceManager::curr_frame_resource_index() const
{
    return curr_frame_resource_index_;
}

int FrameResourceManager::pass_count() const
{
    return pass_count_;
}

int FrameResourceManager::object_count() const
{
    return object_count_;
}

int FrameResourceManager::skinned_mesh_object_count() const
{
    return skinned_mesh_object_count_;
}

FrameResource* FrameResourceManager::GetResource(int index) const
{
    return frame_resources_[index].get();
}

void FrameResourceManager::ResetFrameResources(ID3D12Device* device, UINT cb_pass_count,
	UINT cb_object_count, UINT cb_skinned_mesh_object_count, UINT cb_material_count, UINT cb_ui_mesh_count)
{
    frame_resources_.clear();
    frame_resources_.reserve(kFrameCount);
    curr_frame_resource_index_ = 0;

    for (int i = 0; i < kFrameCount; ++i)
    {
        frame_resources_.push_back(std::make_unique<FrameResource>(
            device, cb_pass_count, (UINT)cb_object_count, 
            (UINT)cb_skinned_mesh_object_count, cb_material_count, cb_ui_mesh_count));
    }

    pass_count_ = cb_pass_count;
    object_count_ = cb_object_count;
    skinned_mesh_object_count_ = cb_skinned_mesh_object_count;
    material_count_ = cb_material_count;

    curr_frame_resource_ = frame_resources_[curr_frame_resource_index_].get();
}

void FrameResourceManager::CirculateFrameResource(ID3D12Fence* fence)
{   
    // 다음 프레임 리소스 인덱스로 이동 (Ring Buffer)
    curr_frame_resource_index_ = (curr_frame_resource_index_ + 1) % kFrameCount;
    curr_frame_resource_ = frame_resources_[curr_frame_resource_index_].get();

    // GPU가 해당 프레임 리소스 사용을 완료했는지 체크
    // fence 값이 0이 아니고(초기상태 제외), GPU의 완료 값(GetCompletedValue)이 목표 값보다 작다면 대기
    if (curr_frame_resource_->fence != 0 && 
        fence->GetCompletedValue() < curr_frame_resource_->fence)
    {
        HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
        fence->SetEventOnCompletion(curr_frame_resource_->fence, event_handle);
        WaitForSingleObject(event_handle, INFINITE);
        CloseHandle(event_handle);
    }

}

void FrameResourceManager::ResetCurrentOffset()
{
	curr_frame_resource_->current_bone_transform_offset = 0;
	curr_frame_resource_->current_instance_offset = 0;
	curr_frame_resource_->current_main_visible_offset = 0;
	curr_frame_resource_->current_shadow_visible_offset = 0;
	curr_frame_resource_->current_ui_offset = 0;
}
