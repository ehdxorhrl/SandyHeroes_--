#include "GlobalValues.hlsl"

struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 position_w : POSITION;
    float3 normal_w : NORMAL;
    float3 tangent_w : TANGENT;
    float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    
    v_out.position_w = mul(float4(v_in.position, 1.f), g_world_matrix).xyz;
    v_out.position = mul(mul(float4(v_out.position_w, 1.f), g_view_matrix), g_projection_matrix);

    v_out.normal_w = mul(v_in.normal, (float3x3) g_world_matrix);
    v_out.tangent_w = mul(v_in.tangent, (float3x3) g_world_matrix);
    v_out.uv = v_in.uv;

    return v_out;
}

float4 PS(VertexOut p_in) : SV_Target
{
    return float4(1, 1, 1, 0.0f);
}
