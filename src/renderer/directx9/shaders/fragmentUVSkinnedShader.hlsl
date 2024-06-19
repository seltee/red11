sampler2D gTexSampler : register(s0) =
                            sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

struct VS_Output
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

float4 DefuseColor : register(c4);
float4 AmbientColor : register(c5);
float4 EmissionColor : register(c6);

float IsDefuseTextureSet : register(c7);

float4 main(VS_Output pin) : SV_TARGET
{
    float4 pixel = IsDefuseTextureSet ? tex2D(gTexSampler, pin.uv) * DefuseColor : DefuseColor;
    pixel *= AmbientColor;
    return pixel;
}