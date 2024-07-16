struct VS_Output
{
    float4 pos : POSITION;
    float2 depth : TEXCOORD1;
};

float4 main(VS_Output pin) : SV_TARGET
{
    float depth = pin.depth.x / pin.depth.y;
    return float4(depth, depth, depth, 1.0f);
}