#include "GlobalValues.hlsl"

struct VertexIn
{
    float3 position : POSITION;
};

struct VertexOut
{
    float4 position : SV_Position;
    float3 position_L : POSITION;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    v_out.position_L = v_in.position;
    
    float3 position_w = mul(float4(v_in.position, 1.f), g_world_matrix).xyz;
    position_w.xyz += g_camera_position;
    v_out.position = mul(mul(float4(position_w, 1.f), g_view_matrix), g_projection_matrix).xyww;
    
    return v_out;
}

float4 PS(VertexOut p_in) : SV_Target
{
    return g_cube_map.Sample(g_linear_warp, p_in.position_L);
}