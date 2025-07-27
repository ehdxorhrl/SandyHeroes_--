
#define MAX_LIGHTS 16
#define DIRECTION_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
    float3 strength;
    float falloff_start;    // 점/스팟 조명
    float3 direction;       // 방향/스팟 조명
    float falloff_end;      // 점/스팟 조명
    float3 position;        // 점/스팟 조명
    float spot_power;       // 스팟 조명
    bool enable;            // 조명의 사용여부(일부조명은 게임 진행에 따라 끄고 킬수 있음)
    int type;               // 0: 방향, 1: 점, 2: 스팟
    float2 pad; 
};

struct Material
{
    float4 albedo_color;    
    float3 fresnel_r0;      // 입사각 0도에서 프레넬 반사율
    float glossiness;       // 광택
    float4 emission_color; // 발산광
};

//점조명과 스팟조명에 사용하는 감쇠계수 계산함수
float CalcAttenuation(float d, float falloff_start, float falloff_end)
{
    return saturate((falloff_end - d) / (falloff_end - falloff_start));
}

//프레넬 반사 값을 구하기위해 슐릭근사를 통해 계산하는 함수
//Rf = R0 + (1 - R0) * ((1 - cos(seta))^5)
float3 SchlickFresnel(float3 r0, float3 normal, float3 light_vector)
{
    float cos_seta = saturate(dot(normal, light_vector));
    float f0 = 1 - cos_seta;
    float3 rf = r0 + (1 - r0) * pow((1 - cos_seta), 5);
    return rf;
}

float3 HalfLambert(float3 v1, float3 v2)
{
    return (dot(v1, v2) * 0.5 + 0.5);
}

//빛의 세기, 빛벡터, 정점의 법선, 정점에서 카메라로 향하는 to eye 벡터, 재질을 받아 조명계산을 하는 함수
float3 BlinnPhongLighting(float3 light_strength, float3 light_vector, float3 normal, float3 to_eye_vector, Material mat)
{
    const float m = mat.glossiness * 256.f;
    float3 half_vector = normalize(to_eye_vector + light_vector);
    
    //표면 거칠기에 의한 계수
    float roughness_factor = (m + 8.0f) * pow(max(dot(half_vector, normal) * 0.5 + 0.5, 0.f), m) / 8.0f;
    //프레넬 효과에 의한 계수
    float3 fresnel_factor = SchlickFresnel(mat.fresnel_r0, half_vector, light_vector);
    
    //위 두 계수를 적용한 반영반사 색상
    float3 specular_albedo = roughness_factor * fresnel_factor;
    
    //반영 반사율을 1 미만으로 낮춤(LDR 렌더링)
    specular_albedo = specular_albedo / (1.f + specular_albedo);
    
    // ambient 조명 연산은 이 함수가 아니라 ps함수 내부에서 처리한다.
    return (mat.albedo_color.rgb + specular_albedo) * light_strength;
}

//방향 조명 계산
float3 ComputeDirectionalLight(Light l, Material mat, float3 normal, float3 to_eye_vector)
{
    float3 light_vector = -l.direction;
    float3 test = HalfLambert(light_vector, normal);
    //test = ceil(test * 5) / 5;
    float3 light_strength = l.strength * max(test, 0.f);
    
    return BlinnPhongLighting(light_strength, light_vector, normal, to_eye_vector, mat);
}

//점 조명 계산
float3 ComputePointLight(Light l, Material mat, float3 position, float3 normal, float3 to_eye_vector)
{
    float3 light_vector = l.position - position;
    
    float d = length(light_vector);
    
    if(d > l.falloff_end)
        return 0.f;
    
    light_vector /= d;
    
    float3 light_strength = l.strength * max(HalfLambert(light_vector, normal), 0.f);

    float attenuation = CalcAttenuation(d, l.falloff_start, l.falloff_end);
    light_strength *= attenuation;
    
    return BlinnPhongLighting(light_strength, light_vector, normal, to_eye_vector, mat);
}

//스팟 조명 계산
float3 ComputeSpotLight(Light l, Material mat, float3 position, float3 normal, float3 to_eye_vector)
{
    float3 light_vector = l.position - position;
    
    float d = length(light_vector);
    
    if (d > l.falloff_end)
        return 0.f;
    
    light_vector /= d;
    
    float3 light_strength = l.strength * max(HalfLambert(light_vector, normal), 0.f);

    float attenuation = CalcAttenuation(d, l.falloff_start, l.falloff_end);
    light_strength *= attenuation;

    float spot_factor = pow(max(dot(-light_vector, l.direction), 0.0f), l.spot_power);
    light_strength *= spot_factor;

    return BlinnPhongLighting(light_strength, light_vector, normal, to_eye_vector, mat);
}

//TODO: 향후 그림자 추가시 shadow 관련 계수 추가
float4 ComputeLighting(Light lights[MAX_LIGHTS], Material mat, 
                        float3 position, float3 normal, float3 to_eye_vector)
{
    float3 result = 0.f;
    
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if(!lights[i].enable)
            continue;
        if(lights[i].type == DIRECTION_LIGHT)
        {
            result += ComputeDirectionalLight(lights[i], mat, normal, to_eye_vector);
        }
        else if(lights[i].type == POINT_LIGHT)
        {
            result += ComputePointLight(lights[i], mat, position, normal, to_eye_vector);
        }
        else if(lights[i].type == SPOT_LIGHT)
        {
            result += ComputeSpotLight(lights[i], mat, position, normal, to_eye_vector);
        }
    }
    
    return float4(result, 0.f);
}