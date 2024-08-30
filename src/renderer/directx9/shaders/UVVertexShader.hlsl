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
    float3 shadowCoord[3] : TEXCOORD3;
};

matrix World : register(c0);
matrix WorldViewProj : register(c4);
matrix WorldInverseTranspose : register(c8);

// Parameters, 0 - z multiplier, 1 - z shift
float4 Parameters : register(c12);

matrix LightsShadowMatricies[3] : register(c16);

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    vout.worldPos = mul(float4(vin.pos, 1.f), World).xyz;
    vout.normal = normalize(mul(vin.normal, (float3x3)WorldInverseTranspose));
    vout.texCoord = vin.texCoord;
    for (int i = 0; i < 3; i++)
    {
        float4 shadowCoord = mul(float4(vout.worldPos, 1.0), LightsShadowMatricies[i]);
        shadowCoord.xyz /= shadowCoord.w;
        shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
        shadowCoord.y = 1.0 - shadowCoord.y;
        vout.shadowCoord[i] = shadowCoord.xyz;
    }
    vout.pos.z *= Parameters[0];
    vout.pos.z += Parameters[1];
    return vout;
}