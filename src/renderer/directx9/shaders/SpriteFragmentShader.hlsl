struct VS_Output
{
    float4 pos : POSITION;
    float2 texCoord : TEXCOORD;
};

float4 color : register(c0);

float4 main(VS_Output pin) : SV_TARGET
{
    return color;
}
