#include "stdafx.h"
#include "AnimatorComponent.h"
#include "Object.h"
#include "AnimationSet.h"
#include "AnimationTrack.h"
#include "AnimationState.h"
#include "HitDragonAnimationState.h"

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

	// 0행렬 초기화
	std::fill(animated_tramsforms_.begin(), animated_tramsforms_.end(), XMFLOAT4X4{});

	int track_state = animation_state_->Run(elapsed_time, owner_, animation_tracks_[track_index_].is_end(), this);
	//int track_state = animation_state_->Run(elapsed_time, owner_, animation_tracks_[0].is_end(), this);

	if (track_state != track_index_)
	{
		change_time_ = 0.f;
		before_track_index_ = track_index_;
		track_index_ = track_state;
		if (before_track_index_ == -1)
		{
			before_track_index_ = track_index_;
		}
		animation_tracks_[track_index_].Start((AnimationLoopType)animation_state_->animation_loop_type());
		animation_tracks_[before_track_index_].Start((AnimationLoopType)animation_state_->animation_loop_type());
	}

	XMFLOAT3 before_root_bone_position = root_bone_frame_->position_vector();

	change_time_ += elapsed_time;
	change_time_ = std::min(change_time_, max_change_time_);
	float position = (max_change_time_ - change_time_) / max_change_time_;

	if (change_time_ < max_change_time_)
	{
		animation_tracks_[before_track_index_].PlayTrack(elapsed_time * speed_scale_, animated_tramsforms_, position);
	}
	animation_tracks_[track_index_].PlayTrack(elapsed_time * speed_scale_, animated_tramsforms_, 1.f - position);

	for (int i = 0; i < bone_frames_.size(); ++i)
	{
		bone_frames_[i]->set_transform_matrix(animated_tramsforms_[i]);
	}

	if (is_root_motion_animation_)
	{
		XMFLOAT3 delta_translation = root_bone_frame_->position_vector() - before_root_bone_position;
		owner_->set_position_vector(owner_->position_vector() + delta_translation);
		root_bone_frame_->set_position_vector(before_root_bone_position);
	}
	if (is_ignore_root_bone_traslation_)
	{
		root_bone_frame_->set_position_vector(before_root_bone_position);
	}

}

void AnimatorComponent::AttachBoneFrames()
{
	bone_frames_.resize(frame_names_.size());
	animated_tramsforms_.resize(frame_names_.size());
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

void AnimatorComponent::set_speed_scale(float value)
{
	speed_scale_ = value;
}

void AnimatorComponent::set_is_ignore_root_bone_traslation(bool value)
{
	is_ignore_root_bone_traslation_ = value;
}

AnimationState* AnimatorComponent::animation_state() const
{
	return animation_state_.get();
}
