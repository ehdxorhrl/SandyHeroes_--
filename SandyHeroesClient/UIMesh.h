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

    //UI 메쉬는 인스턴싱에서 제외되기 때문에 예전함수로 오버라이드 한다.
    virtual void UpdateConstantBuffer(FrameResource* curr_frame_resource, int& cb_index) override;
    virtual void UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& cb_index) override;
    virtual void Render(ID3D12GraphicsCommandList* command_list, int material_index, FrameResource* curr_frame_resource) override;

private:
    XMFLOAT2 screen_position_{};    //ui의 화면 위치
    XMFLOAT2 ui_size_{};        //ui의 화면에서 크기


};

