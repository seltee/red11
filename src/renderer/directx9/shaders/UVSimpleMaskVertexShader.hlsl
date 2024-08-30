struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct VS_Output
{
    float4 pos : POSITION;
    float2 texCoord : TEXCOORD;
};

matrix WorldViewProj : register(c4);

// Parameters, 0 - z multiplier, 1 - z shift
float4 Parameters : register(c12);

VS_Output main(VS_Input vin)
{
    VS_Output vout;
    vout.pos = mul(float4(vin.pos, 1.f), WorldViewProj);
    vout.texCoord = vin.texCoord;

    vout.pos.z *= Parameters[0];
    vout.pos.z += Parameters[1];

    return vout;
}