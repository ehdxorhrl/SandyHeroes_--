#include "stdafx.h"
#include "SkinnedMesh.h"
#include "MeshComponent.h"
//#include "SkinnedMeshComponent.h"
#include "Material.h"
#include "XMathUtil.h"

using namespace file_load_util;
void SkinnedMesh::LoadSkinnedMeshFromFile(std::ifstream& file)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);
	name_ = load_token;

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BonesPerVertex>:");
#endif // _DEBUG
	bones_per_vertex_ = ReadFromFile<int>(file);

	// bone name 정보
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneNames>:");
#endif // _DEBUG

	int bone_count = ReadFromFile<int>(file);
	bone_names_.reserve(bone_count);
	for (int i = 0; i < bone_count; ++i)
	{
		ReadStringFromFile(file, load_token);
		bone_names_.emplace_back(load_token);
	}

	// bone offset 정보
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneOffsets>:");
#endif // _DEBUG

	bone_offsets_.resize(ReadFromFile<int>(file));
	ReadFromFile<XMFLOAT4X4>(file, bone_offsets_.data(), bone_offsets_.size());
	//열 우선 행렬로 변환
	for (XMFLOAT4X4& bone_offset : bone_offsets_)
	{
		XMStoreFloat4x4(&bone_offset, XMMatrixTranspose(XMLoadFloat4x4(&bone_offset)));
	}

	// bone index 정보
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneIndices>:");
#endif // _DEBUG

	bone_indices_.resize(ReadFromFile<int>(file));
	ReadFromFile<XMINT4>(file, bone_indices_.data(), bone_indices_.size());

	// bone weight 정보
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneWeights>:");
#endif // _DEBUG

	bone_weights_.resize(ReadFromFile<int>(file));
	ReadFromFile<XMFLOAT4>(file, bone_weights_.data(), bone_weights_.size());

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "</SkinningInfo>");
#endif // _DEBUG

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<Mesh>:");
#endif // _DEBUG

	LoadMeshFromFile(file);
}
