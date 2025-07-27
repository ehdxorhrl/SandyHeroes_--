#pragma once
#include "MeshComponent.h"
class SkinnedMeshComponent :
    public MeshComponent
{
public:
    SkinnedMeshComponent(Object* owner, Mesh* mesh);
    SkinnedMeshComponent(const SkinnedMeshComponent& other);
    SkinnedMeshComponent& operator=(const SkinnedMeshComponent& rhs);

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index) override;

    // skinned mesh의 bone names 정보를 사용하여 bone_frames_ 배열을 구성한다.
    void AttachBoneFrames(const std::vector<std::string>& bone_names);

private:
    std::vector<Object*> bone_frames_;

    bool is_attached_bone_frames_ = false;

};

