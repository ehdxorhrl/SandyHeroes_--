#include "GlobalValues.hlsl"

struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    v_out.position = float4(v_in.position, 1.f);
    v_out.uv = v_in.uv;
    return v_out;
}

float4 PS(VertexOut v_in) : SV_TARGET
{   
    float4 r_value = g_albedo_map.Sample(g_anisotropic_warp, v_in.uv);
    
    clip(r_value.a - 0.01f);
    
    return r_value;

}
