#pragma once
#include "Shader.h"
class StandardSkinnedMeshShader :
    public Shader
{
public:
    StandardSkinnedMeshShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
    virtual D3D12_BLEND_DESC CreateBlendState() override;

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;

};

