#include "common.hlsl"

sampler2D albedoTexSampler : register(s0);
sampler2D alphaTexSampler : register(s1);
sampler2D emissionTexSampler : register(s2);
sampler2D normalTexSampler : register(s3);
sampler2D metallicTexSampler : register(s4);
sampler2D roughnessTexSampler : register(s5);
sampler2D aoTexSampler : register(s6);
sampler2D emptyTexSampler : register(s7);

sampler2D shadowTexSampler[8] : register(s8);

struct VS_Output
{
    float4 pos : POSITION;
    float3 worldPos : TEXCOORD2;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD;
    float3 shadowCoord[3] : TEXCOORD3;
};

// useAlbedoTexture, useAlphaTexture, useNormalTexture, useMetallicTexture;
float4 MaterialData1 : register(c12);
// useRoughnessTexture, useAOTexture, useEmissionTexture;
float4 MaterialData2 : register(c13);
// roughnessValue, metallicValue, alphaValue, discardLowAlpha;
float4 MaterialData3 : register(c14);

float4 AlbedoColor : register(c15);
float4 EmissionColor : register(c16);
float4 CameraPosition : register(c17);
float4 AmbientLightColor : register(c18);
float4 EngineData : register(c19);

Light Lights[16] : register(c20);

float4 main(VS_Output pin) : SV_TARGET
{
    // Simplified material properties
    float4 albedo = MaterialData1[0] == 1.0f ? tex2D(albedoTexSampler, pin.texCoord) * AlbedoColor : AlbedoColor;
    float alpha = MaterialData1[1] == 1.0f ? tex2D(alphaTexSampler, pin.texCoord).a * MaterialData3[2] : albedo.a * MaterialData3[2];
    if (MaterialData3[3] == 1.0f && alpha < 0.5f)
        discard;
    float metallic = MaterialData1[3] == 1.0f ? tex2D(metallicTexSampler, pin.texCoord).r * MaterialData3[1] : MaterialData3[1];
    float roughness = max(MaterialData2[0] == 1.0f ? tex2D(roughnessTexSampler, pin.texCoord).r * MaterialData3[0] : MaterialData3[0], 0.0001);
    float ao = MaterialData2[1] ? tex2D(aoTexSampler, pin.texCoord).r : 1.0;

    float3 N = normalize(pin.normal);
    float3 V = normalize(CameraPosition.xyz - pin.worldPos.xyz);
    float3 H, F, L;

    float3 diffuse = (float3)albedo / 3.14159265359;

    float3 color = (float3)albedo * (float3)AmbientLightColor * ao;

    if (Lights[0].type[0] > 0.1)
        color += CaclLightWithShadow(
            Lights[0], shadowTexSampler[0], shadowTexSampler[1], diffuse, metallic, roughness, N, V, pin.worldPos, pin.shadowCoord[0], pin.shadowCoord[1], N);

    if (Lights[1].type[0] > 0.1)
        color += CaclLightWithShadow(
            Lights[1], shadowTexSampler[2], shadowTexSampler[3], diffuse, metallic, roughness, N, V, pin.worldPos, pin.shadowCoord[2], pin.shadowCoord[3], N);

    if (Lights[2].type[0] > 0.1)
        color += CaclLightWithShadow(
            Lights[2], shadowTexSampler[4], shadowTexSampler[5], diffuse, metallic, roughness, N, V, pin.worldPos, pin.shadowCoord[4], pin.shadowCoord[5], N);

    if (Lights[3].type[0] > 0.1)
        color += CaclLight(Lights[3], diffuse, metallic, roughness, N, V, N);

    if (Lights[4].type[0] > 0.1)
        color += CaclLight(Lights[4], diffuse, metallic, roughness, N, V, N);

    if (Lights[5].type[0] > 0.1)
        color += CaclLight(Lights[5], diffuse, metallic, roughness, N, V, N);

    if (Lights[6].type[0] > 0.1)
        color += CaclLight(Lights[6], diffuse, metallic, roughness, N, V, N);

    if (Lights[7].type[0] > 0.1)
        color += CaclLight(Lights[7], diffuse, metallic, roughness, N, V, N);

    if (Lights[8].type[0] > 0.1)
        color += CaclLight(Lights[8], diffuse, metallic, roughness, N, V, N);

    if (Lights[9].type[0] > 0.1)
        color += CaclLight(Lights[9], diffuse, metallic, roughness, N, V, N);

    if (Lights[10].type[0] > 0.1)
        color += CaclLight(Lights[10], diffuse, metallic, roughness, N, V, N);

    if (Lights[11].type[0] > 0.1)
        color += CaclLight(Lights[11], diffuse, metallic, roughness, N, V, N);

    if (Lights[12].type[0] > 0.1)
        color += CaclLight(Lights[12], diffuse, metallic, roughness, N, V, N);

    if (Lights[13].type[0] > 0.1)
        color += CaclLight(Lights[13], diffuse, metallic, roughness, N, V, N);

    if (Lights[14].type[0] > 0.1)
        color += CaclLight(Lights[14], diffuse, metallic, roughness, N, V, N);

    if (Lights[15].type[0] > 0.1)
        color += CaclLight(Lights[15], diffuse, metallic, roughness, N, V, N);

    // gamma

    float txShift = EngineData[0] * 12.0f;
    float add = tex2D(emissionTexSampler, pin.texCoord * 0.5 + float2(txShift, txShift)).r * 1.2;
    add *= tex2D(emissionTexSampler, pin.texCoord * 0.5 + float2(txShift * -1.21, -txShift)).r * 1.2;
    add *= tex2D(emissionTexSampler, pin.texCoord * 0.5 + float2(txShift, txShift * -1.31)).r * 1.2;
    add *= tex2D(emissionTexSampler, pin.texCoord * 0.5 + float2(txShift * 0.9, txShift * 1.01 + 0.5)).r * 1.2;
    add *= add;

    color.r = min(1.0, color.r + add);
    color.g = min(1.0, color.g + add);
    color.b = min(1.0, color.b + add * 2.0);

    color = pow(color, 1.0 / 1.2);

    return float4(color, alpha);
}