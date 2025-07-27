#pragma once

class AnimationSet;
class Object;

enum class AnimationLoopType { kLoop, kOnce, kRepeat };

class AnimationTrack
{
public:
	AnimationTrack(AnimationSet* animation_set);

	bool is_end() const;

	void Start();
	void Start(AnimationLoopType loop_type, int repeat_count = 0); //애니메이션을 주어진 루프 방식으로 재생
	void Pause();
	void Stop();

	void PlayTrack(float elapsed_time, std::vector<XMFLOAT4X4>& animated_transforms, float weight);

private:
	//트랙이 재생하는 애니메이션 셋
	AnimationSet* animation_set_;

	AnimationLoopType loop_type_ = AnimationLoopType::kLoop;
	int repeat_count_ = 1; //반복 횟수 Repeat 모드일때만 작동
	int repeat_counter_ = 0;

	bool is_pause_ = false;
	bool is_end_ = false; //애니메이션 재생이 종료되면 true

	float animation_time_ = 0.f;

};

