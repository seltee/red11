#include "common.hlsl"

struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
};

struct VS_Output
{
    float4 pos : POSITION;
    float2 texCoord : TEXCOORD;
};

struct InputSettings
{
    // addX, addY, mulX, mulY
    float4 uvData;
};

matrix WorldViewProj : register(c0);
InputSettings settings : register(c4);

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.texCoord = float2(settings.uvData[0] + vin.texCoord.x * settings.uvData[2], settings.uvData[1] + vin.texCoord.y * settings.uvData[3]);
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    return vout;
}