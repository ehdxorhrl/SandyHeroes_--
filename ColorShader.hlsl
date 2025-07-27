
#include "GlobalValues.hlsl"

struct VertexIn
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    
    v_out.position = mul(mul(mul(float4(v_in.position, 1.f), g_world_matrix), g_view_matrix), g_projection_matrix);
    v_out.color = v_in.color;
    
    return v_out;

}

float4 PS(VertexOut p_in) : SV_Target
{
    return p_in.color;
}