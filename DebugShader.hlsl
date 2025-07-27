#include "GlobalValues.hlsl"

struct VertexIn
{
    float3 position : POSITION;
};

struct VertexOut
{
    float4 position : SV_Position;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    
    v_out.position = mul(mul(mul(float4(v_in.position, 1.f), g_world_matrix), g_view_matrix), g_projection_matrix);
    
    return v_out;
}

float4 PS(VertexOut p_in) : SV_Target
{
    return float4(1, 0, 0, 1);
}
