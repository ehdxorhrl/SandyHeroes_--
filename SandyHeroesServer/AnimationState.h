#pragma once

class Object;
class AnimatorComponent;

//특정 개체의 애니메이션 상태 전이를 정의하는 클래스, Run 함수를 통해 개체의 애니메이션 트랙 index를 선택한다.
class AnimationState
{
public:
	virtual ~AnimationState() {}

	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) = 0;
	virtual int Run(Object* object, bool is_end, AnimatorComponent* animator) = 0;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) = 0;


	virtual AnimationState* GetCopy() = 0;

	virtual int GetDeadAnimationTrack() const;

	void ChangeAnimationTrack(int animation_track, Object* object, AnimatorComponent* animator);

	void set_animation_loop_type(int value);

	int animation_loop_type() const;
	int animation_track() const;

protected:
	void set_animation_track(int value);

	int animation_loop_type_ = 0; // 0: Loop, 1: Once, 2: Repeat
	int repeat_count_ = 0; // 반복 횟수

private:
	int animation_track_ = 0;

};

