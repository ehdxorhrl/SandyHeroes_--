#pragma once
class TextFormat
{
public:
	TextFormat() = default;
	TextFormat(IDWriteFactory* dwrite_factory,
		const std::wstring& font_name = L"맑은 고딕", 
		float font_size = 20.f,
		DWRITE_TEXT_ALIGNMENT text_alignment = DWRITE_TEXT_ALIGNMENT_CENTER, 
		DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_FONT_WEIGHT font_weight = DWRITE_FONT_WEIGHT_NORMAL, 
		DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH font_stretch = DWRITE_FONT_STRETCH_NORMAL, 
		const std::wstring& locale_name = L"ko-kr");
	~TextFormat() {}


	//getter
	IDWriteTextFormat* dwrite_text_format() const { return dwrite_text_format_.Get(); }
	std::wstring font_name() const { return font_name_; }
	float font_size() const { return font_size_; }
	DWRITE_FONT_WEIGHT font_weight() const { return font_weight_; }
	DWRITE_FONT_STYLE font_style() const { return font_style_; }
	DWRITE_FONT_STRETCH font_stretch() const { return font_stretch_; }
	std::wstring locale_name() const { return locale_name_; }

private:
	ComPtr<IDWriteTextFormat> dwrite_text_format_;

	std::wstring font_name_;
	float font_size_ = 20.f;
	DWRITE_TEXT_ALIGNMENT text_alignment_ = DWRITE_TEXT_ALIGNMENT_CENTER;	// 텍스트(가로) 정렬
	DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment_ = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;	// 세로 정렬
	DWRITE_FONT_WEIGHT font_weight_ = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STYLE font_style_ = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH font_stretch_ = DWRITE_FONT_STRETCH_NORMAL;
	std::wstring locale_name_ = L"ko-kr";

};

