#pragma once
#include "Shader.h"

class ParticleShader : public Shader
{
public:
    ParticleShader() { shader_type_ = ShaderType::kParticle; }

    virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature);

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;
    D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** shader_blob);
    virtual D3D12_BLEND_DESC CreateBlendState();
    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
};