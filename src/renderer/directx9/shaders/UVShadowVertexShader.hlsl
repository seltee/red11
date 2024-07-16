struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct VS_Output
{
    float4 pos : POSITION;
    float2 depth : TEXCOORD1;
};

matrix World : register(c0);
matrix WorldViewProj : register(c4);
matrix WorldInverseTranspose : register(c8);

VS_Output main(VS_Input vin)
{
    float4 clipPos = mul(float4(vin.pos.xyz, 1.0), WorldViewProj);

    VS_Output vout;
    vout.pos = clipPos;
    // vout.pos.z *= vout.pos.w;
    // vout.pos.w = 1.0;
    vout.depth = float2(clipPos.z, clipPos.w);
    return vout;
}