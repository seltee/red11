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
};

matrix World : register(c0);
matrix WorldViewProj : register(c4);

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    vout.worldPos = vin.pos;
    return vout;
}
