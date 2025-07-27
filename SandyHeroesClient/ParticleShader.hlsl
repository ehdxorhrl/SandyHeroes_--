#include "GlobalValues.hlsl"

struct VertexIn
{
    float4 Color : COLOR;
    float3 PosW : POSITION;
    float2 Size : SIZE;
};

struct GeoOut
{
    float4 Color : Color;
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};

struct PixelOut
{
    float4 Color : SV_Target;
    //float4 Mask : SV_Target1;
};

VertexIn VS(VertexIn vin)
{
    return vin;
}

[maxvertexcount(4)]
void GS(point VertexIn gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    float3 look = normalize(g_camera_position - gin[0].PosW);
    float3 right = cross(g_camera_up_axis, look);

    float halfWidth = 0.5f * gin[0].Size.x;
    float halfHeight = 0.5f * gin[0].Size.y;

    float4 v[4];
    v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * g_camera_up_axis, 1.0f);
    v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * g_camera_up_axis, 1.0f);
    v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * g_camera_up_axis, 1.0f);
    v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * g_camera_up_axis, 1.0f);

    float2 texC[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    GeoOut gout;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(mul(v[i], g_view_matrix), g_projection_matrix);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;
        gout.TexC = texC[i];
        gout.PrimID = primID;
        gout.Color = gin[0].Color;

        triStream.Append(gout);
    }
}

PixelOut PS(GeoOut pin)
{
    PixelOut pOut;

    pOut.Color = g_albedo_map.Sample(g_linear_warp, pin.TexC) * pin.Color;
   // pOut.Color.a = pOut.Color.r;
    //pOut.Mask = float4(1, 1, 1, 1);

    return pOut;
}
