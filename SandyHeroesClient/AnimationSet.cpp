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

void AnimationSet::AnimateBoneFrame(std::vector<XMFLOAT4X4>& animated_transforms, float animation_time, float weight)
{
	for (int i = 0; i < key_frame_times_.size(); ++i)
	{
		if (animation_time > key_frame_times_[i] && i != key_frame_times_.size() - 1)
			continue;

		float t = (animation_time - key_frame_times_[i - 1]) / (key_frame_times_[i] - key_frame_times_[i - 1]);
		for (int k = 0; k < animated_transforms.size(); ++k)
		{
			if (i == 0)
			{
				animated_transforms[k] = xmath_util_float4x4::Add(animated_transforms[k], key_frame_transforms_[i][k]);
				continue;
			}
			XMVECTOR s1, r1, t1;
			XMVECTOR s2, r2, t2;
			XMMatrixDecompose(&s1, &r1, &t1, XMLoadFloat4x4(&key_frame_transforms_[i - 1][k]));
			XMMatrixDecompose(&s2, &r2, &t2, XMLoadFloat4x4(&key_frame_transforms_[i][k]));
			XMVECTOR S = XMVectorLerp(s1, s2, t);
			XMVECTOR R = XMQuaternionSlerp(r1, r2, t);
			XMVECTOR T = XMVectorLerp(t1, t2, t);

			XMFLOAT4X4 animated_transform;
			XMStoreFloat4x4(&animated_transform, XMMatrixAffineTransformation(S, XMVectorZero(), R, T) * weight);

			animated_transforms[k] = xmath_util_float4x4::Add(animated_transforms[k], animated_transform);
		}
		return;
	}

}
