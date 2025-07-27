#pragma once
#include "InputControllerComponent.h"

//인풋을 받아서 전후좌우상하 이동 및 상하 좌우 회전을 수행하는 단순한 컴포넌트
class TestControllerComponent :
    public InputControllerComponent
{
public:
    TestControllerComponent(Object* owner);
    virtual ~TestControllerComponent() {}

    virtual Component* GetCopy();

    virtual bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) override;

    virtual void Update(float elapsed_time);

	virtual void set_client_wnd(HWND value) 
	{
		client_wnd_ = value;
	}

private:
	HWND client_wnd_{ nullptr };

	//이동속도
	float move_speed_{ 25.f };
	float move_max_speed_{ 70.f };
	float move_min_speed_{ 5.f };

	//회전속도
	float rotate_speed_{ 0.1f };
};

