#include "stdafx.h"
#include "UIMesh.h"
#include "Shader.h"

UIMesh::UIMesh(float screen_x, float screen_y, float screen_width, float screen_height, float z_depth)
    :screen_position_(screen_x, screen_y), ui_size_(screen_width, screen_height)
{
    positions_.reserve(4);

    float left = 0.f;
    float top = 0.f;
    float right = screen_width;
    float bottom = screen_height;

    positions_.push_back(XMFLOAT3{ left, top, z_depth });
    positions_.push_back(XMFLOAT3{ right, top, z_depth });
    positions_.push_back(XMFLOAT3{ left, bottom, z_depth });
    positions_.push_back(XMFLOAT3{ right, bottom, z_depth });

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

UIMesh::UIMesh(float screen_width, float screen_height, float z_depth)
    :screen_position_(0.f, 0.f), ui_size_(screen_width, screen_height)
{
    positions_.reserve(4);

    float left = 0.f;
    float top = 0.f;
    float right = screen_width;
    float bottom = screen_height;

    positions_.push_back(XMFLOAT3{ left, top, z_depth });
    positions_.push_back(XMFLOAT3{ right, top, z_depth });
    positions_.push_back(XMFLOAT3{ left, bottom, z_depth });
    positions_.push_back(XMFLOAT3{ right, bottom, z_depth });

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

XMFLOAT2 UIMesh::screen_position() const
{
    return screen_position_;
}

XMFLOAT2 UIMesh::ui_size() const
{
    return ui_size_;
}
