#pragma once
#include "Shader.h"
class SkyboxShader :
    public Shader
{
public:
    SkyboxShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob);
    virtual D3D12_RASTERIZER_DESC CreateRasterizerState() override;
    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState() override;
};

