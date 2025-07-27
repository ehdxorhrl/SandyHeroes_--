#include "stdafx.h"
#include "AnimatorComponent.h"
#include "Object.h"
#include "AnimationSet.h"
#include "AnimationTrack.h"
#include "AnimationState.h"

AnimatorComponent::AnimatorComponent(Object* owner, 
	const std::vector<std::unique_ptr<AnimationSet>>& animation_sets, 
	const std::vector<std::string>& frame_names,
	const std::string& root_bone_name,
	AnimationState* animation_state) : Component(owner)
{
	animation_tracks_.reserve(animation_sets.size());
	for (int i = 0; i < animation_sets.size(); ++i)
	{
		animation_tracks_.emplace_back(animation_sets[i].get());
	}
	frame_names_ = frame_names;
	root_bone_name_ = root_bone_name;
	animation_state_.reset(animation_state);
}

AnimatorComponent::AnimatorComponent(const AnimatorComponent& other) : Component(other), 
	animation_tracks_(other.animation_tracks_), frame_names_(other.frame_names_), root_bone_name_(other.root_bone_name_)
{
	animation_state_.reset(other.animation_state_->GetCopy());
}

Component* AnimatorComponent::GetCopy()
{
	return new AnimatorComponent(*this);
}

void AnimatorComponent::Update(float elapsed_time)
{
	if (!is_attached_bone_frames_)
		AttachBoneFrames();

	int track_state = animation_state_->Run(owner_, animation_tracks_[track_index_].is_end()); 

	if (track_state != track_index_)
	{
		track_index_ = track_state;
		animation_tracks_[track_index_].Start(AnimationLoopType::kLoop);
	}

	animation_tracks_[track_index_].PlayTrack(elapsed_time, bone_frames_);

	if (is_root_motion_animation_)
	{
		//TODO: animation track 구현 후 loop 상태의 애니메이션의 root motion 구현
		XMFLOAT3 delta_translation = root_bone_frame_->position_vector() - before_translation_;
		owner_->set_position_vector(owner_->position_vector() + delta_translation);
		before_translation_ = root_bone_frame_->position_vector();
	}
	root_bone_frame_->set_position_vector(XMFLOAT3{ 0,0,0 });


}

void AnimatorComponent::AttachBoneFrames()
{
	bone_frames_.resize(frame_names_.size());
	for (int i = 0; i < frame_names_.size(); ++i)
	{
		bone_frames_[i] = owner_->FindFrame(frame_names_[i]);
	}
	root_bone_frame_ = owner_->FindFrame(root_bone_name_);
}


void AnimatorComponent::set_animation_state(AnimationState* value)
{
	animation_state_.reset(value);
}

AnimationState* AnimatorComponent::animation_state() const
{
	return animation_state_.get();
}
