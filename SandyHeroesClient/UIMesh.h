#pragma once
#include "Mesh.h"

//화면상에 2d ui를 그리는 메쉬
class UIMesh :
    public Mesh
{
public:
    //pivot 은 사각형의 왼쪽 위이다.
    UIMesh(float screen_x, float screen_y, float screen_width, float screen_height, float z_depth = 0.f);
    UIMesh(float screen_width, float screen_height, float z_depth = 0.f);
    ~UIMesh() {}

    XMFLOAT3 ScreenToNDC(float x, float y, float width, float height);

    //getter
    XMFLOAT2 screen_position() const;
    XMFLOAT2 ui_size() const;

private:
    XMFLOAT2 screen_position_{};    //ui의 화면 위치
    XMFLOAT2 ui_size_{};        //ui의 화면에서 크기


};

