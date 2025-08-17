#include "stdafx.h"
#include "AnimationTrack.h"
#include "AnimationSet.h"

AnimationTrack::AnimationTrack(AnimationSet* animation_set) :
	animation_set_(animation_set)
{
}

bool AnimationTrack::is_end() const
{
	return is_end_;
}

void AnimationTrack::Start()
{
	is_pause_ = false;
	is_end_ = false;
}

void AnimationTrack::Start(AnimationLoopType loop_type, int repeat_count)
{
	loop_type_ = loop_type; 
	repeat_count_ = repeat_count;
	repeat_counter_ = 0;
	animation_time_ = 0.f;
	Start();
}

void AnimationTrack::Pause()
{
	is_pause_ = true;
}

void AnimationTrack::Stop()
{
	animation_time_ = 0.f;
	is_pause_ = true;
}

void AnimationTrack::PlayTrack(float elapsed_time, std::vector<XMFLOAT4X4>& animated_transforms, float weight)
{
	if (is_pause_ || is_end_)
		return;

	animation_time_ += elapsed_time;

	switch (loop_type_)
	{
	case AnimationLoopType::kLoop:
		while (animation_time_ > animation_set_->total_time())
			animation_time_ -= animation_set_->total_time();
		break;
	case AnimationLoopType::kOnce:
		if (animation_time_ > animation_set_->total_time()) {
			animation_time_ = animation_set_->total_time();
			animation_set_->AnimateBoneFrame(animated_transforms, animation_time_, weight);
			is_end_ = true;
			return;
		}
		break;
	case AnimationLoopType::kRepeat:
		if (animation_time_ > animation_set_->total_time()) {
			++repeat_counter_;
			if (repeat_counter_ >= repeat_count_) {
				animation_time_ = animation_set_->total_time();
				animation_set_->AnimateBoneFrame(animated_transforms, animation_time_, weight);
				is_end_ = true;
				return;
			}
			animation_time_ -= animation_set_->total_time();
		}
		break;
	default:
		break;
	}

	animation_set_->AnimateBoneFrame(animated_transforms, animation_time_, weight);

}
