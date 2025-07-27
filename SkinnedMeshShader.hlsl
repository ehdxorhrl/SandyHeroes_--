#include "GlobalValues.hlsl"


struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    int4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 position_w : POSITIONT;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    
    //모델 좌표계에서 월드 좌표계로 변환하는 행렬
    float4x4 vertex_to_world_matrix = (float4x4) 0.0f;
    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        vertex_to_world_matrix += v_in.weights[i] * mul(g_bone_offset_matrix[v_in.indices[i]], g_bone_transform_matrix[v_in.indices[i]]);
    }
    
    v_out.position_w = mul(float4(v_in.position, 1.f), vertex_to_world_matrix).xyz;

    v_out.position = mul(mul(float4(v_out.position_w, 1.f), g_view_matrix), g_projection_matrix);
    
    v_out.normal = mul(v_in.normal, (float3x3)vertex_to_world_matrix);
    
    v_out.uv = v_in.uv;
    
    return v_out;
}

float4 PS(VertexOut p_in) : SV_Target
{
    float4 diffuse_albedo = float4(0.5, 0.5, 0.5, 1) * g_material.albedo_color;
    
    p_in.normal = normalize(p_in.normal);
    
    float3 to_eye_vector = normalize(g_camera_position - p_in.position_w);
    
    float4 ambient = g_ambient_light * diffuse_albedo;
    Material mat = { diffuse_albedo, g_material.fresnel_r0, g_material.glossiness, g_material.emission_color };
    float4 direct_light = ComputeLighting(g_lights, mat, p_in.position_w, p_in.normal, to_eye_vector);
    
    
    float4 result = ambient + direct_light + float4(g_material.emission_color, 1.f);
    result.a = diffuse_albedo.a;
    
    return result;
}