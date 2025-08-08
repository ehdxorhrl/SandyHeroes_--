#pragma once
#include "Component.h"
class RazerComponent :
	public Component
{
public:
	RazerComponent(Object* owner);
	RazerComponent(Object* owner, XMFLOAT3 start, XMFLOAT3 end);
	RazerComponent(const RazerComponent& other);
	virtual ~RazerComponent() {}
	virtual Component* GetCopy() override;
	virtual void Update(float elapsed_time) override;

	void InitRazer(XMFLOAT3 start, XMFLOAT3 end);

	float life_time() const { return life_time_; } // 레이저의 생명 시간 반환

	//getter
	XMFLOAT3 start_position() const { return start_position_; } // 레이저 시작 위치 반환
	XMFLOAT3 end_position() const { return end_position_; } // 레이저 끝 위치 반환
	float damage() const { return damage_; } // 레이저가 입히는 피해량 반환
	float is_collision_active() const { return is_collision_active_; } // 레이저 충돌 활성화 상태 반환
	bool is_collided() const { return is_collided_; } // 레이저가 충돌했는지 여부 반환

	//setter
	void set_is_collided(bool value) { is_collided_ = value; }

private:
	float life_time_ = 0.0f; // 레이저의 생명 시간
	float max_life_time_ = 0.8f; // 레이저의 최대 생명 시간
	XMFLOAT3 start_position_{ 0,0,0 }; // 레이저 시작 위치
	XMFLOAT3 end_position_{ 0,0,0 }; // 레이저 끝 위치
	float damage_ = 10.f; // 레이저가 입히는 피해량
	bool is_collision_active_ = false; // 레이저가 충돌 활성화 상태인지 여부
	bool is_collided_ = false; // 레이저가 충돌했는지 여부
};

