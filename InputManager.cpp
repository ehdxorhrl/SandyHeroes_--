#include "stdafx.h"
#include "InputManager.h"
#include "InputControllerComponent.h"

InputManager* InputManager::kInputManager = nullptr;

InputManager::InputManager()
{
	assert(!kInputManager);
	kInputManager = nullptr;
}

void InputManager::EnQueueInputMessage(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	if (IsFull())
		return;

	message_buffer_[tail_].id = message_id;
	message_buffer_[tail_].w_param = w_param;
	message_buffer_[tail_].l_param = l_param;
	message_buffer_[tail_].time = message_time;
	tail_ = (tail_ + 1) % kMaxInputMessage;
}

InputMessage InputManager::DeQueueInputMessage(float play_time)
{
	if(IsEmpty())
		return InputMessage();

	InputMessage input_message = message_buffer_[head_];
	head_ = (head_ + 1) % kMaxInputMessage;
	while (play_time - input_message.time > kExpirationDateInputMessage)
	{
		if (IsEmpty())
			return InputMessage();
		input_message = message_buffer_[head_];
		head_ = (head_ + 1) % kMaxInputMessage;
	}
	return input_message;
}

bool InputManager::IsEmpty()
{
	return head_ == tail_;
}

bool InputManager::IsFull()
{
	return (tail_ + 1) % kMaxInputMessage == head_;
}
