#include "stdafx.h"
#include "ShadowShader.h"

D3D12_INPUT_LAYOUT_DESC ShadowShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE ShadowShader::CreateVertexShader(ID3DBlob** shader_blob)
{
    return CompileShaderFromFile(L"Shadow.hlsl", "MeshVS", "vs_5_1", shader_blob);
}

D3D12_SHADER_BYTECODE ShadowShader::CreatePixelShader(ID3DBlob** shader_blob)
{
    return CompileShaderFromFile(L"Shadow.hlsl", "PS", "ps_5_1", shader_blob);
}

D3D12_RASTERIZER_DESC ShadowShader::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC rasterizer_desc{};
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.DepthBias = 1000;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.SlopeScaledDepthBias = 1.0f;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return rasterizer_desc;
}