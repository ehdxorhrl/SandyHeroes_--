#include "LightingUtil.hlsl"

cbuffer ObjectInfo : register(b0)
{
    matrix g_world_matrix;
    float g_object_time;
    float3 o_padding;
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

cbuffer UiInfo : register(b3)
{
    float2 g_screen_offset; // 좌상단 스크린 좌표 (픽셀)
    float g_width_ratio;    // ui 크기 배율 0.0 ~ 1.0
    float g_height_ratio; 
    float2 g_ui_texture_offset;
    float g_ui_layer; // ui 레이어 
    float g_ui_alpha; 
    float2 g_ui_gage_value; // 0.0 ~ 1.0
}

// 프레임 당 1번 갱신되는 상수 버퍼
cbuffer RenderPass : register(b4)
{
    matrix g_view_matrix;
    matrix g_projection_matrix;
    float3 g_camera_position;
    float g_time;
    float4 g_ambient_light;
    Light g_lights[MAX_LIGHTS];
    float2 g_screen_size;
    float2 pad_2;
    float3 g_camera_up_axis;
}

#define TEXTURE_MASK_ALBEDO (1 << 0)
#define TEXTURE_MASK_SPECGLOS (1 << 1)
#define TEXTURE_MASK_METALGLOS (1 << 2)
#define TEXTURE_MASK_EMISSION (1 << 3)
#define TEXTURE_MASK_NORMAL (1 << 4)
#define TEXTURE_MASK_CUBE (1 << 5)

cbuffer Material : register(b5)
{
    Material g_material;
    int g_texture_mask;
}

cbuffer ShadowPass : register(b6)
{
    float4x4 light_view;
    float4x4 light_proj;
    float4x4 shadow_transform;
    float3 light_dir;
    float near_z;
    float3 light_pos_w;
    float far_z;
}

Texture2D g_albedo_map : register(t0);
Texture2D g_spec_glos_map : register(t1);
Texture2D g_metal_glos_map : register(t2);
Texture2D g_emission_map : register(t3);
Texture2D g_normal_map : register(t4);
TextureCube g_cube_map : register(t5);
Texture2D g_shadow_map : register(t6);

SamplerState g_anisotropic_warp : register(s0);
SamplerState g_linear_warp : register(s1);
SamplerComparisonState g_shadow_sampler : register(s2);

//그림자 연산
float CalcShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    g_shadow_map.GetDimensions(0, width, height, numMips);
    
    // Texel size.
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };
    
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += g_shadow_map.SampleCmpLevelZero(g_shadow_sampler, shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / 9.0f;
}
