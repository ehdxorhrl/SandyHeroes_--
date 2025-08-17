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
    
    float3 scaled_pos = v_in.position;
    scaled_pos.x *= g_width_ratio;
    scaled_pos.y *= g_height_ratio;
    
    scaled_pos.xy += g_screen_offset;
    
    // ½ºÅ©¸° ¡æ NDC
    float ndcX = (scaled_pos.x / g_screen_size.x) * 2.0f - 1.0f;
    float ndcY = 1.0f - (scaled_pos.y / g_screen_size.y) * 2.0f;
    
    v_out.position = float4(ndcX, ndcY, g_ui_layer + v_in.position.z, 1.f);
    v_out.uv = v_in.uv + g_ui_texture_offset;
    saturate(v_out.uv);
    
    return v_out;
}

float4 PS(VertexOut v_in) : SV_TARGET
{   
    if (v_in.uv.x > g_ui_gage_value.x)
        discard;
    if (v_in.uv.y < (1.0 - g_ui_gage_value.y))
        discard;

    float4 r_value = g_albedo_map.Sample(g_anisotropic_warp, v_in.uv);
    
    clip(r_value.a - 0.01f);
    
    r_value.a *= g_ui_alpha;
    
    return r_value;

}
