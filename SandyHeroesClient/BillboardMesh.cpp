#include "stdafx.h"
#include "BillboardMesh.h"
#include "Object.h"
#include "FrameResource.h"
#include "GameFramework.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "MeshComponent.h"

BillboardMesh::BillboardMesh()
{
	mesh_type_ = MeshType::kBillboardMesh;
}

