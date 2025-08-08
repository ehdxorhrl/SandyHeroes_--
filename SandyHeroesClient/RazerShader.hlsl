#include "GlobalValues.hlsl"

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 position_w : POSITION0;
    float3 normal_w : NORMAL;
    float3 tangent_w : TANGENT;
    float2 uv : TEXCOORD;
    float4 position_s : POSITION1;
};


float4 PS(VertexOut input) : SV_Target
{
    float3 color = float3(1.0, 0.f, 0.f); // 붉은 레이저
    float3 white = float3(0.1, 0.1, 0.1);

    // Input: UV.y는 레이저의 길이 방향, UV.x는 너비 방향
        float2 uv = input.uv; // uv.x: 0~1 (폭), uv.y: 0~1 (길이)

        // 중앙에서 멀어질수록 투명해짐
            float distanceFromCenter = abs(uv.x - 0.5f); // 0~0.5
            float alpha = saturate(1.0 - distanceFromCenter * 2.0); // 강한 falloff

            float speed = 1.5f; // 내려가는 속도 (단위: UV.y/sec)
            float bandPos = frac(1.0 - (g_object_time * speed)); // 0~1 사이 반복

            // 픽셀이 중심선보다 아래에 있으면 바로 버림 (투명)
            if (uv.y < bandPos)
                discard;

            // 시간 기반 빛 이동 효과
            float scroll = abs(uv.y - bandPos); // 아래로 이동
            float glow = exp(-pow(scroll * 8.0f, 2)); // Gaussian pulse
            alpha *= glow;

            // 투명도 threshold 컷
                if (alpha < 0.05)
                    discard;

                // 색상 출력
                    return float4(color - white * alpha, alpha);
}