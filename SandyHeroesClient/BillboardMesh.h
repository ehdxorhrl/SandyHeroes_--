#pragma once
#include "Mesh.h"
class BillboardMesh :
    public Mesh
{
 public:
    BillboardMesh() {}
	virtual ~BillboardMesh() override {}

    virtual void UpdateConstantBuffer(FrameResource* curr_frame_resource, int& start_index) override;
    virtual void UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& start_index) override;

};

