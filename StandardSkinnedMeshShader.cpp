#include "stdafx.h"
#include "StandardSkinnedMeshShader.h"

StandardSkinnedMeshShader::StandardSkinnedMeshShader()
{
	shader_type_ = ShaderType::kStandardSkinnedMesh;
}

D3D12_INPUT_LAYOUT_DESC StandardSkinnedMeshShader::CreateInputLayout()
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
		{"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 4, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}

	};
	r_value.NumElements = 6;
	r_value.pInputElementDescs = elements;

	return r_value;
}

D3D12_SHADER_BYTECODE StandardSkinnedMeshShader::CreateVertexShader(ID3DBlob** shader_blob)
{
	return(CompileShaderFromFile(L"StandardShader.hlsl", "SkinnedMeshVS", "vs_5_1", shader_blob));
}

D3D12_SHADER_BYTECODE StandardSkinnedMeshShader::CreatePixelShader(ID3DBlob** shader_blob)
{
	return(CompileShaderFromFile(L"StandardShader.hlsl", "PS", "ps_5_1", shader_blob));
}
