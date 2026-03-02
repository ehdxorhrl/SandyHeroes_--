#include "stdafx.h"
#include "TextComponent.h"
#include "TextFormat.h"
#include "Object.h"
#include "GameFramework.h"


TextComponent::TextComponent()
{
    type_ = UiType::kText;
}

TextComponent::TextComponent(Object* owner)
	: UiComponent(owner)
{
    type_ = UiType::kText;
}


TextComponent::TextComponent(
	Object* owner, 
	TextFormat* text_format, 
	D2D1_RECT_F text_rect, 
	std::function<std::wstring(Object*)> get_text_func)
	: UiComponent(owner), text_format_(text_format), text_rect_(text_rect)
{
	get_text_func_ = get_text_func;
	if (get_text_func_)
	{
		is_static_ = false; // get_text_func_ Ǹ  ؽƮ 
	}
}

TextComponent::TextComponent(
	Object* owner, 
	TextFormat* text_format, 
	D2D1_RECT_F text_rect, 
	const std::wstring& text)
	: UiComponent(owner), text_format_(text_format), text_rect_(text_rect), text_(text)
{
}

Component* TextComponent::GetCopy()
{
    return new TextComponent(*this);
}

void TextComponent::Update(float elapsed_time)
{
	GameFramework::Instance()->text_renderer()->AddText(this); // ؽƮ  ߰

	if(is_static_)
	{
		return; //  ؽƮ Ʈ 
	}

	auto locked_owner = owner_.lock();

	if (!get_text_func_)
	{
		// get_text_func_     ޽ 
		if (!locked_owner)
		{
			OutputDebugString(L"TextComponent: owner_ is nullptr!\n");
			return;
		}
		//  ̸ ͼ  ޽ 
		std::string name = locked_owner->name();
		std::wstring wname(name.begin(), name.end());
		wname = L"TextComponent: " + wname + L"'s get_text_func_ is not set!\n";
		OutputDebugString(wname.c_str());
		return;
	}

	auto locked_view = view_.lock();
	if(locked_view)
	{
		text_ = get_text_func_(locked_view.get()); //  Ʈ ؽƮ  Լ ȣ

	}
	else
	{
		if(!locked_owner) return;
		text_ = get_text_func_(locked_owner.get()); //  Ʈ ؽƮ  Լ ȣ
	}

}

void TextComponent::set_text(const std::wstring& text)
{
	text_ = text;

}

void TextComponent::set_get_text_func(const std::function<std::wstring(Object*)>& func)
{
	get_text_func_ = func;
}

void TextComponent::Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush)
{
	d2d_device_context->SetTransform(transform_);
	brush->SetColor(color_);
	d2d_device_context->DrawText(
		text_.c_str(),
		text_.size(),
		text_format_->dwrite_text_format(),
		&text_rect_,
		brush
	);

}
