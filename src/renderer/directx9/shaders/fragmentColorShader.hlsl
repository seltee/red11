struct VS_Output
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(VS_Output pin) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0);
}