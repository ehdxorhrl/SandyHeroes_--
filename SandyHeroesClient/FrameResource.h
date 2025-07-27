#pragma once

#include "UploadBuffer.h"

// 쉐이더에서 사용하는 조명정보를 저장하는 구조체
struct LightInfo
{
	XMFLOAT3 strength;
	float falloff_start;	// 점/스팟 조명
	XMFLOAT3 direction;		// 방향/스팟 조명
	float falloff_end;		// 점/스팟 조명
	XMFLOAT3 position;		// 점/스팟 조명
	float spot_power;		// 스팟 조명
	bool enable;			// 조명의 사용여부(일부조명은 게임 진행에 따라 끄고 킬수 있음)
	int type;				// 0: 방향, 1: 점, 2: 스팟
	XMFLOAT2 pad;
};

// 렌더패스 1회에 사용되는 상수 버퍼
struct CBPass
{
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 proj_matrix;
	XMFLOAT3 camera_position;
	float total_time;
	XMFLOAT4 ambient_light;
	LightInfo lights[kMaxLights];    
	XMFLOAT2 screen_size;
	XMFLOAT2 pad_2;
	XMFLOAT3 camera_up_axis;
};

struct CBShadow
{
	XMFLOAT4X4 light_view;
	XMFLOAT4X4 light_proj;
	XMFLOAT4X4 shadow_transform;
	XMFLOAT3 light_dir;
	float near_z;
	XMFLOAT3 light_pos_w;
	float far_z;
};

// 일반 메쉬를 사용하는 오브젝트의 상수 버퍼
struct CBObject
{
	XMFLOAT4X4 world_matrix;
};

// 스킨 메쉬를 사용하는 오브젝트의 bone transform 행렬
struct CBBoneTransform
{
	XMFLOAT4X4 bone_transform_matrix[kMaxBoneCount];
};

struct CBMaterial
{
	XMFLOAT4 albedo_color;
	XMFLOAT3 fresnel_r0;
	float glossiness;
	XMFLOAT4 emission_color;
	int texture_mask;
};

struct CBUi
{
	XMFLOAT2 screen_offset;	// 좌상단 스크린 좌표 (픽셀)
	float width_ratio;		// ui 크기 배율 0.0 ~ 1.0
	float height_ratio;
	XMFLOAT2 texture_offset; // ui 텍스쳐 오프셋 (0.0 ~ 1.0)
	float ui_layer;			// ui 레이어 
	float padding;	// padding to make it 16 bytes aligned
	XMFLOAT2 gage_value; // ui 가젯 값 (0.0 ~ 1.0)
};

// 게임에서 한 프레임에 사용하는 리소스에 대한 구조체
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT pass_count,
		UINT object_count, UINT skinned_mesh_object_count, UINT material_count, UINT ui_mesh_count)
	{
		device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&d3d_allocator));

		cb_pass = std::make_unique<UploadBuffer<CBPass>>(device, pass_count, true);
		cb_shadow = std::make_unique<UploadBuffer<CBShadow>>(device, pass_count, true);
		cb_object = std::make_unique<UploadBuffer<CBObject>>(device, object_count, true);
		cb_bone_transform = std::make_unique<UploadBuffer<CBBoneTransform>>(device, skinned_mesh_object_count, true);
		cb_material = std::make_unique<UploadBuffer<CBMaterial>>(device, material_count, true);
		cb_ui = std::make_unique<UploadBuffer<CBUi>>(device, ui_mesh_count, true);
	}
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource() {}

	ComPtr<ID3D12CommandAllocator> d3d_allocator;

	std::unique_ptr<UploadBuffer<CBPass>> cb_pass;
	std::unique_ptr<UploadBuffer<CBShadow>> cb_shadow;
	std::unique_ptr<UploadBuffer<CBObject>> cb_object;
	std::unique_ptr<UploadBuffer<CBBoneTransform>> cb_bone_transform;
	std::unique_ptr<UploadBuffer<CBMaterial>> cb_material;
	std::unique_ptr<UploadBuffer<CBUi>> cb_ui;

	UINT64 fence = 0;
};

