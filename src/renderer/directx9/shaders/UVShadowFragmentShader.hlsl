struct VS_Output
{
    float4 pos : POSITION;
    float depth : TEXCOORD1;
};

float4 main(VS_Output pin) : SV_TARGET
{
    return float4(pin.depth, pin.depth, pin.depth, 1.0f);
}