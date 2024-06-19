struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_Output
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

matrix WorldViewProj : register(c0);

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    vout.uv = vin.uv;
    return vout;
}