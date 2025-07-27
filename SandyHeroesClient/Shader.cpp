#include "stdafx.h"
#include "Shader.h"
#include "Material.h"
#include "FrameResource.h"
#include "DescriptorManager.h"
#include "CameraComponent.h"


D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
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

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC blend_desc{};
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].LogicOpEnable = FALSE;
	blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blend_desc);

}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
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

D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(WCHAR* file_name, LPCSTR shader_name, 
	LPCSTR shader_profile, ID3DBlob** shader_blob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(file_name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		shader_name, shader_profile, nCompileFlags, 0, shader_blob, &pd3dErrorBlob);
	char* pErrorString;
	if (pd3dErrorBlob)
	{
		pErrorString = (char*)pd3dErrorBlob->GetBufferPointer();
	}

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*shader_blob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*shader_blob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

void Shader::CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
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
	pipeline_state_desc.NumRenderTargets = 1;
	pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipeline_state_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipeline_state_desc.SampleDesc.Count = 1;
	pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&d3d_pipeline_state_));

	if (d3d_vertex_shader_blob) d3d_vertex_shader_blob->Release();
	if (d3d_pixel_shader_blob) d3d_pixel_shader_blob->Release();

	if (pipeline_state_desc.InputLayout.pInputElementDescs) delete pipeline_state_desc.InputLayout.pInputElementDescs;

}

ID3D12PipelineState* Shader::GetPipelineState() const
{
	return d3d_pipeline_state_.Get();
}

ShaderType Shader::shader_type() const
{
	return shader_type_;
}

void Shader::ReserveMaterials(UINT capacity)
{
	materials_.reserve(capacity);
}

void Shader::AddMaterial(Material* material)
{
	materials_.push_back(material);
	material->set_shader_type((int)shader_type_);
}

void Shader::Render(ID3D12GraphicsCommandList* command_list, 
	FrameResource* curr_frame_resource, DescriptorManager* descriptor_manager, CameraComponent* camera, bool bShadow)
{
	if(!is_render_)
		return;

	if(!bShadow)
		command_list->SetPipelineState(d3d_pipeline_state_.Get());

	for (const auto& const material : materials_)
	{
		if(is_frustum_culling_)
			material->Render(command_list, curr_frame_resource, descriptor_manager, camera, bShadow);
		else
		{
			material->Render(command_list, curr_frame_resource, descriptor_manager, nullptr, bShadow);
		}
	}
}
