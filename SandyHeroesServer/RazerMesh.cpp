#include "stdafx.h"
#include "RazerMesh.h"

RazerMesh::RazerMesh()
{
	name_ = "RazerMesh";
}

RazerMesh::RazerMesh(float width, float height)
{
	name_ = "RazerMesh";

	positions_.reserve(4);
	positions_.emplace_back(XMFLOAT3(-width / 2.f, 0, 0.0f));
	positions_.emplace_back(XMFLOAT3(width / 2.f, 0, 0.0f));
	positions_.emplace_back(XMFLOAT3(width / 2.f, height, 0.0f));
	positions_.emplace_back(XMFLOAT3(-width / 2.f, height, 0.0f));

	uvs_.reserve(4);
	uvs_.emplace_back(XMFLOAT2(0.0f, 0.0f));
	uvs_.emplace_back(XMFLOAT2(1.0f, 0.0f));
	uvs_.emplace_back(XMFLOAT2(1.0f, 1.0f));
	uvs_.emplace_back(XMFLOAT2(0.0f, 1.0f));

	indices_array_.reserve(1);
	std::vector<UINT> indices{ 0, 1, 2, 0, 2, 3 };
	indices_array_.emplace_back(std::move(indices));


	bounds_ = BoundingBox(
		XMFLOAT3(-width / 2.f, 0.0f, 0.0f),
		XMFLOAT3(width / 2.f, height, 0.0f)
	);
}
