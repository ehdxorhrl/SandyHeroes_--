#include "stdafx.h"
#include "UIMesh.h"
#include "Shader.h"

UIMesh::UIMesh(float screen_x, float screen_y, float screen_width, float screen_height)
    :screen_position_(screen_x, screen_y), screen_size_(screen_width, screen_height)
{
    shader_type_ = (int)ShaderType::kUI;

    positions_.reserve(4);

    float left = screen_x;
    float top = screen_y;
    float right = screen_x + screen_width;
    float bottom = screen_y + screen_height;

    positions_.push_back(ScreenToNDC(left, top, kDefaultFrameBufferWidth, kDefaultFrameBufferHeight));
    positions_.push_back(ScreenToNDC(right, top, kDefaultFrameBufferWidth, kDefaultFrameBufferHeight));
    positions_.push_back(ScreenToNDC(left, bottom, kDefaultFrameBufferWidth, kDefaultFrameBufferHeight));
    positions_.push_back(ScreenToNDC(right, bottom, kDefaultFrameBufferWidth, kDefaultFrameBufferHeight));

    uvs_.reserve(4);
    uvs_.push_back(XMFLOAT2{ 0.f, 0.f });
    uvs_.push_back(XMFLOAT2{ 1.f, 0.f });
    uvs_.push_back(XMFLOAT2{ 0.f, 1.f });
    uvs_.push_back(XMFLOAT2{ 1.f, 1.f });

    indices_array_.resize(1);

    auto& index_array = indices_array_[0];

    //»ï°¢Çü 2°³
    index_array.reserve(6);

    index_array.insert(index_array.end(), { 0,1,2,1,3,2 });
}

XMFLOAT3 UIMesh::ScreenToNDC(float x, float y, float width, float height)
{
	return XMFLOAT3{
        (x / width) * 2.0f - 1.0f,
        1.0f - (y / height) * 2.0f,
        0.0f
    };
}
