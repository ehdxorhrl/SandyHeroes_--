#include "stdafx.h"
#include "BreathingShader.h"

D3D12_INPUT_LAYOUT_DESC BreathingShader::CreateInputLayout()
{
    D3D12_INPUT_LAYOUT_DESC r_value;

    D3D12_INPUT_ELEMENT_DESC* elements = new D3D12_INPUT_ELEMENT_DESC[]{
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    r_value.NumElements = 4;
    r_value.pInputElementDescs = elements;

    return r_value;
}

D3D12_SHADER_BYTECODE BreathingShader::CreateVertexShader(ID3DBlob** shader_blob)
{
    return CompileShaderFromFile(L"Breathing.hlsl", "VS", "vs_5_1", shader_blob);
}

D3D12_SHADER_BYTECODE BreathingShader::CreatePixelShader(ID3DBlob** shader_blob)
{
    return CompileShaderFromFile(L"Breathing.hlsl", "PS", "ps_5_1", shader_blob);
}

D3D12_BLEND_DESC BreathingShader::CreateBlendState()
{
    D3D12_BLEND_DESC blend_desc{};
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;

    auto& rt = blend_desc.RenderTarget[0];
    rt.BlendEnable = TRUE;
    rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    rt.BlendOp = D3D12_BLEND_OP_ADD;
    rt.SrcBlendAlpha = D3D12_BLEND_ONE;
    rt.DestBlendAlpha = D3D12_BLEND_ZERO;
    rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return blend_desc;
}
