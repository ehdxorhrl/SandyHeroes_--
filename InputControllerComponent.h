#pragma once
#include "Component.h"

// 인풋을 처리하는 컨트롤러의 상위 클래스
// 이 클래스를 상속받아 ProcessInput 함수에서 인풋을 처리할 수 있다.
class InputControllerComponent :
    public Component
{
public:
    InputControllerComponent(Object* owner);
    virtual ~InputControllerComponent() {}

    virtual Component* GetCopy() = 0;

    virtual bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) = 0;

    void set_client_wnd(HWND value);

protected:
    std::unordered_map<char, bool> is_key_down_;
    POINT mouse_xy_{ 0,0 };

    HWND client_wnd_{ nullptr };
};

