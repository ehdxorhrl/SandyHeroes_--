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

    //setter
    void set_animation_state(AnimationState* value);
    void set_speed_scale(float value);
    void set_is_ignore_root_bone_traslation(bool value);

    //getter
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
    bool is_root_motion_animation_ = false;
    bool is_ignore_root_bone_traslation_ = false; // true면 root_bone에 적용되는 애니메이션의 translation 값이 무시됨

    //애니메이션 재생 속도
    float speed_scale_ = 1.f;
};

