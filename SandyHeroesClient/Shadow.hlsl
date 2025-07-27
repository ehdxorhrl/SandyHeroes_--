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
    float3 position_w : POSITION;
    float3 normal_w : NORMAL;
    float3 tangent_w : TANGENT;
    float2 uv : TEXCOORD;
};

VertexOut MeshVS(MeshVertexIn v_in)
{
    VertexOut v_out;
    
    v_out.position_w = mul(float4(v_in.position, 1.f), g_world_matrix).xyz;
    v_out.position = mul(mul(float4(v_out.position_w, 1.f), light_view), light_proj);
        
    //비균등 비례가 월드행렬에 있다면 월드행렬의 역전치 행렬로 변환해야함!
    v_out.normal_w = mul(v_in.normal, (float3x3) g_world_matrix);
    v_out.tangent_w = mul(v_in.tangent, (float3x3) g_world_matrix);

    v_out.uv = v_in.uv;

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
    
    v_out.position_w = mul(float4(v_in.position, 1.f), vertex_to_world_matrix).xyz;

    v_out.position = mul(mul(float4(v_out.position_w, 1.f), light_view), light_proj);
    
    v_out.normal_w = mul(v_in.normal, (float3x3) vertex_to_world_matrix);
    v_out.tangent_w = mul(v_in.tangent, (float3x3) vertex_to_world_matrix);

    v_out.uv = v_in.uv;
    
    return v_out;
}

void PS(VertexOut p_in)
{
    
}


