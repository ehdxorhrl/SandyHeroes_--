#include "LightingUtil.hlsl"

cbuffer ObjectInfo : register(b0)
{
    matrix g_world_matrix;
}

#define SKINNED_ANIMATION_BONES 128
#define MAX_BONE_PER_VERTEX 4

cbuffer BoneTransforms : register(b1)
{
    matrix g_bone_transform_matrix[SKINNED_ANIMATION_BONES];
}

cbuffer BoneOffsets : register(b2)
{
    matrix g_bone_offset_matrix[SKINNED_ANIMATION_BONES];
}

// 프레임 당 1번 갱신되는 상수 버퍼
cbuffer RenderPass : register(b3)
{
    matrix g_view_matrix;
    matrix g_projection_matrix;
    float3 g_camera_position;
    float pad;
    float4 g_ambient_light;
    Light g_lights[MAX_LIGHTS];
}

#define TEXTURE_MASK_ALBEDO (1 << 0)
#define TEXTURE_MASK_SPECGLOS (1 << 1)
#define TEXTURE_MASK_METALGLOS (1 << 2)
#define TEXTURE_MASK_EMISSION (1 << 3)
#define TEXTURE_MASK_NORMAL (1 << 4)
#define TEXTURE_MASK_CUBE (1 << 5)

cbuffer Material : register(b4)
{
    Material g_material;
    int g_texture_mask;
}

Texture2D g_albedo_map : register(t0);
Texture2D g_spec_glos_map : register(t1);
Texture2D g_metal_glos_map : register(t2);
Texture2D g_emission_map : register(t3);
Texture2D g_normal_map : register(t4);
TextureCube g_cube_map : register(t5);

SamplerState g_anisotropic_warp : register(s0);
SamplerState g_linear_warp : register(s1);
