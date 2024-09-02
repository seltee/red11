#include "common.hlsl"

struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VS_Output
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 shadowCoord[6] : TEXCOORD2;
};

matrix ViewProj : register(c0);

// Parameters, 0 - z multiplier, 1 - z shift
float4 Parameters : register(c12);

// 16 - 40
matrix LightsShadowMatricies[6] : register(c16);

// 40 - 256 - bones data = 54 bones
matrix BoneMatrices[54] : register(c40);

VS_Output main(VS_Input vin)
{
    float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 normal = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        int boneIndex = int(vin.boneIndices[i]);
        float weight = vin.boneWeights[i];

        float4x4 world = BoneMatrices[boneIndex];

        position += weight * mul(float4(vin.pos, 1.0f), world);
        normal += weight * mul(vin.normal, (float3x3)world);
    }

    VS_Output vout;

    vout.pos = mul(position, ViewProj);
    vout.worldPos = position.xyz;
    vout.normal = normalize(normal);
    vout.texCoord = vin.texCoord;

    for (int p = 0; p < 6; p++)
    {
        float4 shadowCoord = mul(float4(vout.worldPos, 1.0), LightsShadowMatricies[p]);
        shadowCoord.xyz /= shadowCoord.w;
        shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
        shadowCoord.y = 1.0 - shadowCoord.y;
        vout.shadowCoord[p] = shadowCoord.xyz;
    }

    vout.pos.z *= Parameters[0];
    vout.pos.z += Parameters[1];
    return vout;
}