#include "stdafx.h"
#include "TextRenderer.h"
#include "TextComponent.h"

void TextRenderer::Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush)
{
	if (!is_render_ || text_list_.empty())
	{
		return; // 렌더링할 텍스트가 없으면 반환
	}

	for (auto& text_component : text_list_)
	{
		text_component->Render(d2d_device_context, brush); // 텍스트 렌더링
	}	

	text_list_.clear(); // 렌더링 후 텍스트 리스트 초기화

}
