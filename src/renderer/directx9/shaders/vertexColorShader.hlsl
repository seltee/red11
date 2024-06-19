struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : POSITION;
    float4 color : COLOR;
};

struct VS_Output
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

cbuffer cbPerFrame : register(c0)
{
    matrix WorldViewProj : register(c0);
};

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    vout.color = vin.color;
    return vout;
}