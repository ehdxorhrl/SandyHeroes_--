#include "stdafx.h"
#include "TextComponent.h"
#include "TextFormat.h"
#include "Object.h"

TextRenderer* TextComponent::kTextRenderer = nullptr;

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
		is_static_ = false; // get_text_func_가 설정되면 동적 텍스트로 간주
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
	kTextRenderer->AddText(this); // 텍스트 렌더러에 추가

	if(is_static_)
	{
		return; // 정적 텍스트는 업데이트하지 않음
	}
	if (!get_text_func_)
	{
		// get_text_func_가 설정되지 않은 경우 디버그 메시지 출력
		if (!owner_)
		{
			OutputDebugString(L"TextComponent: owner_ is nullptr!\n");
			return;
		}
		// 오너의 이름을 가져와서 디버그 메시지 출력
		std::string name = owner_->name();
		std::wstring wname(name.begin(), name.end());
		wname = L"TextComponent: " + wname + L"'s get_text_func_ is not set!\n";
		OutputDebugString(wname.c_str());
		return;
	}

	if(view_)
	{
		text_ = get_text_func_(view_); // 뷰 오브젝트에서 텍스트를 가져오는 함수 호출

	}
	else
	{
		text_ = get_text_func_(owner_); // 오너 오브젝트에서 텍스트를 가져오는 함수 호출
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
