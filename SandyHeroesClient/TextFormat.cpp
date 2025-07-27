#include "stdafx.h"
#include "TextFormat.h"

TextFormat::TextFormat(IDWriteFactory* dwrite_factory, 
	const std::wstring& font_name, float font_size, 
	DWRITE_TEXT_ALIGNMENT text_alignment,
	DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment,
	DWRITE_FONT_WEIGHT font_weight, DWRITE_FONT_STYLE font_style, 
	DWRITE_FONT_STRETCH font_stretch, const std::wstring& locale_name)
	: font_name_(font_name), font_size_(font_size), 
	text_alignment_(text_alignment), paragraph_alignment_(paragraph_alignment),
	  font_weight_(font_weight), font_style_(font_style), 
	font_stretch_(font_stretch), locale_name_(locale_name)
{
	dwrite_factory->CreateTextFormat(
		font_name.c_str(),
		NULL,
		font_weight,
		font_style,
		font_stretch,
		font_size,
		locale_name.c_str(),
		&dwrite_text_format_
	);
	dwrite_text_format_->SetTextAlignment(text_alignment);
	dwrite_text_format_->SetParagraphAlignment(paragraph_alignment);
}
