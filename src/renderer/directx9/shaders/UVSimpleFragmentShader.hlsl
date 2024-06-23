struct VS_Output
{
    float4 pos : POSITION;
};

float4 main(VS_Output pin) : SV_TARGET
{
    return float4(0.5f, 0.5f, 0.5f, 1.0f);
}