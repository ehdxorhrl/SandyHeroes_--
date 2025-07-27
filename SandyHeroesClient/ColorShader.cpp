#include "stdafx.h"
#include "ColorShader.h"

ColorShader::ColorShader() 
{
	shader_type_ = ShaderType::kColor;
}

D3D12_INPUT_LAYOUT_DESC ColorShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC r_value;

	D3D12_INPUT_ELEMENT_DESC* elements = new D3D12_INPUT_ELEMENT_DESC[2]{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	r_value.NumElements = 2;
	r_value.pInputElementDescs = elements;

	return r_value;
}

D3D12_SHADER_BYTECODE ColorShader::CreateVertexShader(ID3DBlob** shader_blob)
{
	return(CompileShaderFromFile(L"ColorShader.hlsl", "VS", "vs_5_1", shader_blob));
}

D3D12_SHADER_BYTECODE ColorShader::CreatePixelShader(ID3DBlob** shader_blob)
{
	return(CompileShaderFromFile(L"ColorShader.hlsl", "PS", "ps_5_1", shader_blob));
}
