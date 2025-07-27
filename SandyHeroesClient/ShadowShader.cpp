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
    rasterizer_desc.DepthBias = 10000;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.SlopeScaledDepthBias = 1.0f;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return rasterizer_desc;
}

void ShadowShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
    ID3DBlob* d3d_vertex_shader_blob = NULL, * d3d_pixel_shader_blob = NULL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc;
    ::ZeroMemory(&pipeline_state_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    pipeline_state_desc.pRootSignature = root_signature;
    pipeline_state_desc.VS = CreateVertexShader(&d3d_vertex_shader_blob);
    pipeline_state_desc.PS = CreatePixelShader(&d3d_pixel_shader_blob);
    pipeline_state_desc.RasterizerState = CreateRasterizerState();
    pipeline_state_desc.BlendState = CreateBlendState();
    pipeline_state_desc.DepthStencilState = CreateDepthStencilState();
    pipeline_state_desc.InputLayout = CreateInputLayout();
    pipeline_state_desc.SampleMask = UINT_MAX;
    pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipeline_state_desc.NumRenderTargets = 0;
    pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    pipeline_state_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pipeline_state_desc.SampleDesc.Count = 1;
    pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult = device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&d3d_pipeline_state_));

    if (d3d_vertex_shader_blob) d3d_vertex_shader_blob->Release();
    if (d3d_pixel_shader_blob) d3d_pixel_shader_blob->Release();

    if (pipeline_state_desc.InputLayout.pInputElementDescs) delete pipeline_state_desc.InputLayout.pInputElementDescs;
}
