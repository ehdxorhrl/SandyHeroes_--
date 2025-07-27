#include "GlobalValues.hlsl"

struct MeshVertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct SkinnedMeshVertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 position_w : POSITION0;
    float3 normal_w : NORMAL;
    float3 tangent_w : TANGENT;
    float2 uv : TEXCOORD;
    float4 position_s : POSITION1;
};

VertexOut MeshVS(MeshVertexIn v_in)
{
    VertexOut v_out;
    
    float4 position_w = mul(float4(v_in.position, 1.f), g_world_matrix);
    v_out.position_w = position_w.xyz;
    v_out.position = mul(mul(float4(v_out.position_w, 1.f), g_view_matrix), g_projection_matrix);

    //비균등 비례가 월드행렬에 있다면 월드행렬의 역전치 행렬로 변환해야함!
    v_out.normal_w = mul(v_in.normal, (float3x3)g_world_matrix);
    v_out.tangent_w = mul(v_in.tangent, (float3x3) g_world_matrix);

    v_out.uv = v_in.uv;
    
    v_out.position_s = mul(position_w, shadow_transform);

    return v_out;
}

VertexOut SkinnedMeshVS(SkinnedMeshVertexIn v_in)
{
    VertexOut v_out;
    
    //모델 좌표계에서 월드 좌표계로 변환하는 행렬
    float4x4 vertex_to_world_matrix = (float4x4) 0.0f;
    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        vertex_to_world_matrix += v_in.weights[i] * mul(g_bone_offset_matrix[v_in.indices[i]], g_bone_transform_matrix[v_in.indices[i]]);
    }
    
    float4 position_w = mul(float4(v_in.position, 1.f), vertex_to_world_matrix);

    v_out.position_w = position_w.xyz;

    v_out.position = mul(mul(float4(v_out.position_w, 1.f), g_view_matrix), g_projection_matrix);
    
    v_out.normal_w = mul(v_in.normal, (float3x3) vertex_to_world_matrix);
    v_out.tangent_w = mul(v_in.tangent, (float3x3) vertex_to_world_matrix);

    v_out.uv = v_in.uv;
    v_out.position_s = mul(position_w, shadow_transform);
    
    return v_out;
}

float4 PS(VertexOut p_in) : SV_Target
{
    const float fog_start = 40.f;
    const float fog_range = 120.f;
    const float4 fog_color = float4(1.0, 0.8, 0.6, 1.f);
    float dist_to_eye = length(g_camera_position - p_in.position_w);

    //조명연산이 안개에 의해 가려지면 조명연산을 하지 않음
    if(dist_to_eye >= fog_range)
        return fog_color;
    
    float4 diffuse_albedo = g_material.albedo_color;
    if(g_texture_mask & TEXTURE_MASK_ALBEDO)
    {
        diffuse_albedo = g_albedo_map.Sample(g_anisotropic_warp, p_in.uv) * g_material.albedo_color;
    }
    
    clip(diffuse_albedo.a - 0.01);
    
    float3 fresnel_r0 = g_material.fresnel_r0;
    float glossiness = g_material.glossiness;
    if (g_texture_mask & TEXTURE_MASK_SPECGLOS)
    {
        float4 spec_glos = g_spec_glos_map.Sample(g_anisotropic_warp, p_in.uv);
        fresnel_r0 = spec_glos.rgb * g_material.fresnel_r0;
        glossiness = spec_glos.a * g_material.glossiness;

    }
    if (g_texture_mask & TEXTURE_MASK_METALGLOS)
    {
        float4 metal_glos = g_metal_glos_map.Sample(g_anisotropic_warp, p_in.uv);
        float metallic = metal_glos.r;
        fresnel_r0 = (diffuse_albedo * (metallic + (1.f - metallic) * 0.04)).rgb;
        glossiness = metal_glos.a;
    }
    
    float4 emission_color = g_material.emission_color;
    if (g_texture_mask & TEXTURE_MASK_EMISSION)
    {
        emission_color = g_emission_map.Sample(g_anisotropic_warp, p_in.uv);
    }
       
    p_in.normal_w = normalize(p_in.normal_w);
    if(g_texture_mask & TEXTURE_MASK_NORMAL)
    {
        float4 normal_sample = g_normal_map.Sample(g_anisotropic_warp, p_in.uv);
        float3 T = normalize(p_in.tangent_w - dot(p_in.tangent_w, p_in.normal_w) * p_in.normal_w);
        float3 B = cross(p_in.normal_w, T);
        float3x3 tbn = { T, B, p_in.normal_w };
        normal_sample.rgb = 2.f * normal_sample.rgb - 1.f;
        p_in.normal_w = mul(normal_sample.rgb, tbn);

    }
    float shadow_factor = min(CalcShadowFactor(p_in.position_s) + 0.5f , 1.0f);
    
    float3 to_eye_vector = normalize(g_camera_position - p_in.position_w);
    
    float4 ambient = g_ambient_light * diffuse_albedo;
    Material mat = { diffuse_albedo, fresnel_r0, glossiness, emission_color };
    float4 direct_light = ComputeLighting(g_lights, mat, p_in.position_w, p_in.normal_w, to_eye_vector) * shadow_factor;
    
    float4 lit_color = ambient + direct_light + emission_color;
    float4 result = lit_color;
    
    float fog_amount = saturate((dist_to_eye - fog_start) / fog_range);
    
    
    result = lerp(lit_color, fog_color, fog_amount);
    
    result.a = diffuse_albedo.a;
    
    return result;
}


