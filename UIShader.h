#pragma once
#include "Shader.h"
class UIShader :
    public Shader
{
public:
    UIShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;	
    virtual D3D12_BLEND_DESC CreateBlendState()override;
    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState()override;

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;

};

