#pragma once

class Object;

//특정 개체의 애니메이션 상태 전이를 정의하는 클래스, Run 함수를 통해 개체의 애니메이션 트랙 index를 선택한다.
class AnimationState
{
public:
	virtual ~AnimationState() {}

	virtual int Run(Object* object, bool is_end) = 0;
	virtual AnimationState* GetCopy() = 0;

	void set_animation_track(int value);

protected:
	int animation_track_ = 0;

};

