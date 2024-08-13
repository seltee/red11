#include "common.hlsl"

sampler2D albedoTexSampler : register(s0);
sampler2D alphaTexSampler : register(s1);
sampler2D emissionTexSampler : register(s2);
sampler2D normalTexSampler : register(s3);
sampler2D metallicTexSampler : register(s4);
sampler2D roughnessTexSampler : register(s5);
sampler2D aoTexSampler : register(s6);
sampler2D emptyTexSampler : register(s7);

struct VS_Output
{
    float4 pos : POSITION;
    float3 worldPos : TEXCOORD2;
};

float4 main(VS_Output pin) : SV_TARGET
{
    float3 normal = normalize(pin.worldPos);
    float2 uv = SampleSphericalMap(normal);
    float3 color = tex2D(emissionTexSampler, float2(0.0, 1.0) - uv).rgb;
    return float4(color, 1.0f);
}
