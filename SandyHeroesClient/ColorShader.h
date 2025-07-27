#pragma once
#include "Shader.h"

// 정점의 색상정보로 solid하게 그리는 간단한 쉐이더
// wireframe을 원한다면 CreateRasterizerState 함수를 override하라
class ColorShader :
    public Shader
{
public:
    ColorShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;

};

