#pragma once
#include "Shader.h"
class DebugShader :
    public Shader
{
public:
    DebugShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
    virtual D3D12_RASTERIZER_DESC CreateRasterizerState() override;

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;


private:

};

