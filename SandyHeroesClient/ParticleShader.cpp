#include "stdafx.h"
#include "ParticleShader.h"

void ParticleShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	ID3DBlob* d3d_vertex_shader_blob = NULL, * d3d_pixel_shader_blob = NULL, *d3d_geometry_shader_blob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc;
	::ZeroMemory(&pipeline_state_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	pipeline_state_desc.pRootSignature = root_signature;
	pipeline_state_desc.VS = CreateVertexShader(&d3d_vertex_shader_blob);
	pipeline_state_desc.GS = CreateGeometryShader(&d3d_geometry_shader_blob);
	pipeline_state_desc.PS = CreatePixelShader(&d3d_pixel_shader_blob);
	pipeline_state_desc.RasterizerState = CreateRasterizerState();
	pipeline_state_desc.BlendState = CreateBlendState();
	pipeline_state_desc.DepthStencilState = CreateDepthStencilState();
	pipeline_state_desc.InputLayout = CreateInputLayout();
	pipeline_state_desc.SampleMask = UINT_MAX;
	pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	pipeline_state_desc.NumRenderTargets = 1;
	pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipeline_state_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipeline_state_desc.SampleDesc.Count = 1;
	pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&d3d_pipeline_state_));

	if (d3d_vertex_shader_blob) d3d_vertex_shader_blob->Release();
	if (d3d_pixel_shader_blob) d3d_pixel_shader_blob->Release();
	if (d3d_geometry_shader_blob) d3d_geometry_shader_blob->Release();

	if (pipeline_state_desc.InputLayout.pInputElementDescs) delete pipeline_state_desc.InputLayout.pInputElementDescs;

}

D3D12_INPUT_LAYOUT_DESC ParticleShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC r_value;

	D3D12_INPUT_ELEMENT_DESC* elements = new D3D12_INPUT_ELEMENT_DESC[]{
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	r_value.NumElements = 3;
	r_value.pInputElementDescs = elements;

	return r_value;
}

D3D12_BLEND_DESC ParticleShader::CreateBlendState()
{
	D3D12_BLEND_DESC blend_desc{};
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].LogicOpEnable = FALSE;
	blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blend_desc);
}

D3D12_DEPTH_STENCIL_DESC ParticleShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
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

D3D12_SHADER_BYTECODE ParticleShader::CreateVertexShader(ID3DBlob** shader_blob)
{
	return (CompileShaderFromFile(L"ParticleShader.hlsl", "VS", "vs_5_1", shader_blob));
}

D3D12_SHADER_BYTECODE ParticleShader::CreatePixelShader(ID3DBlob** shader_blob)
{
	return (CompileShaderFromFile(L"ParticleShader.hlsl", "PS", "ps_5_1", shader_blob));;
}

D3D12_SHADER_BYTECODE ParticleShader::CreateGeometryShader(ID3DBlob** shader_blob)
{
	return (CompileShaderFromFile(L"ParticleShader.hlsl", "GS", "gs_5_1", shader_blob));;
}
