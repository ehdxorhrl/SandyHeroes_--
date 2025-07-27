#pragma once
#include "Component.h"
#include "AnimationTrack.h"

class AnimationState;

//오브젝트에 애니메이션을 적용하는 컴포넌트
class AnimatorComponent :
    public Component
{
public:
    AnimatorComponent(Object* owner,
        const std::vector<std::unique_ptr<AnimationSet>>& animation_sets,
        const std::vector<std::string>& frame_names,
        const std::string& root_bone_name,
        AnimationState* animation_state);
    
    AnimatorComponent(const AnimatorComponent& other);
    ~AnimatorComponent() {}
    
    virtual Component* GetCopy() override;
    virtual void Update(float elapsed_time) override;

    void AttachBoneFrames();

    void set_animation_state(AnimationState* value);
    AnimationState* animation_state() const;

private:
    std::unique_ptr<AnimationState> animation_state_;
    
    float track_index_ = 0;

    //적용할 애니메이션 목록
    std::vector<AnimationTrack> animation_tracks_;
    std::vector<std::string> frame_names_;
    std::string root_bone_name_;

    //애니메이션이 적용되는 뼈대
    std::vector<Object*> bone_frames_;
    Object* root_bone_frame_;
    bool is_attached_bone_frames_ = false;

    //활성화 시 스킨메쉬의 루트본에 가해지는 translation 값이 root_node에 적용됨
    //비 활성화시 무시됨
    bool is_root_motion_animation_ = false; 
    XMFLOAT3 before_translation_{ 0,0,0 }; // 이전 프레임의 root_bone translation 값
};

