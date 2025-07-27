#include "stdafx.h"
#include "SkyboxShader.h"

SkyboxShader::SkyboxShader()
{
	shader_type_ = ShaderType::kSkybox;
}

D3D12_INPUT_LAYOUT_DESC SkyboxShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC r_value;

	D3D12_INPUT_ELEMENT_DESC* elements = new D3D12_INPUT_ELEMENT_DESC[]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	r_value.NumElements = 2;
	r_value.pInputElementDescs = elements;

	return r_value;
}

D3D12_RASTERIZER_DESC SkyboxShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	rasterizer_desc.ForcedSampleCount = 0;
	rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return rasterizer_desc;
}

D3D12_DEPTH_STENCIL_DESC SkyboxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0x00;
	depth_stencil_desc.StencilWriteMask = 0x00;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	depth_stencil_desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return depth_stencil_desc;
}

D3D12_SHADER_BYTECODE SkyboxShader::CreateVertexShader(ID3DBlob** shader_blob)
{
	return (CompileShaderFromFile(L"SkyboxShader.hlsl", "VS", "vs_5_1", shader_blob));
}

D3D12_SHADER_BYTECODE SkyboxShader::CreatePixelShader(ID3DBlob** shader_blob)
{
	return (CompileShaderFromFile(L"SkyboxShader.hlsl", "PS", "ps_5_1", shader_blob));;
}
