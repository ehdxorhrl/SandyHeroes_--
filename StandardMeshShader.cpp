#include "stdafx.h"
#include "StandardMeshShader.h"

StandardMeshShader::StandardMeshShader()
{
	shader_type_ = ShaderType::kStandardMesh;
}

D3D12_INPUT_LAYOUT_DESC StandardMeshShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE StandardMeshShader::CreateVertexShader(ID3DBlob** shader_blob)
{
	return(CompileShaderFromFile(L"StandardShader.hlsl", "MeshVS", "vs_5_1", shader_blob));
}

D3D12_SHADER_BYTECODE StandardMeshShader::CreatePixelShader(ID3DBlob** shader_blob)
{
	return(CompileShaderFromFile(L"StandardShader.hlsl", "PS", "ps_5_1", shader_blob));
}
