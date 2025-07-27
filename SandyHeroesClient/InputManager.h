#pragma once

class InputControllerComponent;

struct InputMessage
{
	UINT id;
	WPARAM w_param;
	LPARAM l_param;
	float time;
};

//인풋 메시지를 관리하는 클래스
class InputManager
{
public:
	InputManager();
	~InputManager() {}

	void EnQueueInputMessage(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time);
	InputMessage DeQueueInputMessage(float play_time);

	bool IsEmpty();
	bool IsFull();

private:
	static InputManager* kInputManager;

	static constexpr UINT kMaxInputMessage = 1000;
	static constexpr float kExpirationDateInputMessage = 2.f; // 인풋 메시지의 유효기간(단위: 초)

	std::array<InputMessage, kMaxInputMessage> message_buffer_;

	//처리해야할 첫번째 메시지 인덱스
	int head_ = 0;
	//메시지가 추가되는 위치 인덱스
	int tail_ = 0;

};

