#pragma once
#include "Shader.h"

class SkinnedShadowShader : public Shader
{
public:
    SkinnedShadowShader() { shader_type_ = ShaderType::kSkinnedShadow; }

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;
    virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
};