#pragma once

class Object;

class AnimationSet
{
public:
	AnimationSet() {}
	~AnimationSet() {}

	float total_time() const;

	//frame_count: 이 애니메이션을 사용하는 모델의 root 노드를 제외한 프레임 개수
	void LoadAnimationSetFromFile(std::ifstream& file, int frame_count); 

	void AnimateBoneFrame(const std::vector<Object*>& bone_frames, float animation_time);

private:
	std::string name_;

	float total_time_; 	//애니메이션 시간(단위: 초)
	int frame_per_sec_;
	int key_frame_count_; //애니메이션 키 프레임 개수

	std::vector<float> key_frame_times_; //애니메이션의 각 키 프레임의 시간(단위: 초)
	std::vector<std::vector<XMFLOAT4X4>> key_frame_transforms_; //각 키 프레임에서 transform 행렬들

};

