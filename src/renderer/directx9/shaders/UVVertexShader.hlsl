#include "common.hlsl"

struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct VS_Output
{
    float4 pos : POSITION;
    float3 worldPos : TEXCOORD2;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD;
    float3 shadowCoord[4] : TEXCOORD3;
};

matrix World : register(c0);
matrix WorldViewProj : register(c4);
matrix WorldInverseTranspose : register(c8);

matrix LightsShadowMatricies[4] : register(c52);

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    vout.worldPos = mul(float4(vin.pos, 1.f), World).xyz;
    vout.normal = normalize(mul(vin.normal, (float3x3)WorldInverseTranspose));
    vout.texCoord = vin.texCoord;
    for (int i = 0; i < 4; i++)
    {
        float4 shadowCoord = mul(float4(vout.worldPos, 1.0), LightsShadowMatricies[i]);
        shadowCoord.z *= 0.1;
        vout.shadowCoord[i] = shadowCoord.xyz;
    }
    return vout;
}