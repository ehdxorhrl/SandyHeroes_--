#pragma once

class TextComponent;

class TextRenderer
{
public:
	TextRenderer() = default;

	void AddText(TextComponent* text_component)
	{
		text_list_.push_back(text_component);
	}

	void Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush);

	void set_is_render(bool value)
	{
		is_render_ = value;
	}

private:
	std::list<TextComponent*> text_list_;

	bool is_render_ = true;

};

