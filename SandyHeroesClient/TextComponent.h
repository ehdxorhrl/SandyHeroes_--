#pragma once
#include "UiComponent.h"
#include "TextRenderer.h"

class TextFormat;

class TextComponent :
    public UiComponent
{
public:	
	TextComponent();
	TextComponent(Object* owner);
	TextComponent(
		Object* owner,
		TextFormat* text_format = nullptr,
		D2D1_RECT_F text_rect = D2D1::RectF(0.f, 0.f, 1920.f, 1080.f),
		std::function<std::wstring(Object*)> get_text_func = nullptr);
	TextComponent(
		Object* owner,
		TextFormat* text_format = nullptr,
		D2D1_RECT_F text_rect = D2D1::RectF(0.f, 0.f, 1920.f, 1080.f),
		const std::wstring& text = L"None");

	TextComponent(const TextComponent& other) = default; //복사 생성자
	
	virtual Component* GetCopy() override;
	virtual void Update(float elapsed_time);

	void Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush);

	void set_text(const std::wstring& text);
	void set_get_text_func(const std::function<std::wstring(Object*)>& func);
	void set_is_static(bool value) { is_static_ = value; }
	void set_color(const D2D1_COLOR_F& color) { color_ = color; }

	static TextRenderer* kTextRenderer;

private:
	std::wstring text_ = L"None"; //텍스트 내용
	D2D1_COLOR_F color_ = D2D1::ColorF(D2D1::ColorF::White); //폰트 색상
	D2D1::Matrix3x2F transform_ = D2D1::Matrix3x2F::Identity(); //텍스트 변환 행렬
	TextFormat* text_format_ = nullptr; //텍스트 포맷
	bool is_static_{ true }; //텍스트가 정적(static)인지 여부
	std::function<std::wstring(Object*)> get_text_func_{ nullptr }; //텍스트를 가져오는 함수

	D2D1_RECT_F text_rect_{ 0.f, 0.f, 1920.f, 1080.f }; //텍스트 렌더링 영역
};

