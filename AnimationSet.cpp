#include "stdafx.h"
#include "AnimationSet.h"
#include "Object.h"

using namespace file_load_util;
float AnimationSet::total_time() const
{
	return total_time_;
}
void AnimationSet::LoadAnimationSetFromFile(std::ifstream& file, int frame_count)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);
	name_ = load_token;
	total_time_ = ReadFromFile<float>(file);
	frame_per_sec_ = ReadFromFile<int>(file);
	key_frame_count_ = ReadFromFile<int>(file);

	key_frame_times_.resize(key_frame_count_);
	key_frame_transforms_.resize(key_frame_count_);
	for (int i = 0; i < key_frame_count_; ++i)
	{
		ReadStringFromFile(file, load_token); // "<Transforms>:"
		ReadFromFile<int>(file); //index
		key_frame_times_[i] = ReadFromFile<float>(file);
		key_frame_transforms_[i].resize(frame_count);
		ReadFromFile<XMFLOAT4X4>(file, key_frame_transforms_[i].data(), key_frame_transforms_[i].size());
	}

}

void AnimationSet::AnimateBoneFrame(const std::vector<Object*>& bone_frames, float animation_time)
{
	for (int i = 0; i < key_frame_times_.size(); ++i)
	{
		if (animation_time > key_frame_times_[i])
			continue;

		float t = (animation_time - key_frame_times_[i - 1]) / (key_frame_times_[i] - key_frame_times_[i - 1]);
		for (int k = 0; k < bone_frames.size(); ++k)
		{
			XMFLOAT4X4 animated_transform;
			animated_transform = xmath_util_float4x4::Interpolate(key_frame_transforms_[i - 1][k], key_frame_transforms_[i][k], t);
			bone_frames[k]->set_transform_matrix(animated_transform);
		}
		return;
	}

}
