sampler2D albedoTexSampler : register(s0);
sampler2D alphaTexSampler : register(s1);

struct VS_Output
{
    float4 pos : POSITION;
    float2 texCoord : TEXCOORD;
    float2 depth : TEXCOORD1;
};

// useAlbedoTexture, useAlphaTexture, useNormalTexture, useMetallicTexture;
float4 MaterialData1 : register(c12);
// useRoughnessTexture, useAOTexture, useEmissionTexture;
// float4 MaterialData2 : register(c13);
// roughnessValue, metallicValue, alphaValue, discardLowAlpha;
float4 MaterialData3 : register(c14);

float4 AlbedoColor : register(c15);

float4 main(VS_Output pin) : SV_TARGET
{
    // Simplified material properties
    float4 albedo = MaterialData1[0] == 1.0f ? tex2D(albedoTexSampler, pin.texCoord) * AlbedoColor : AlbedoColor;
    float alpha = MaterialData1[1] == 1.0f ? tex2D(alphaTexSampler, pin.texCoord).a * MaterialData3[2] : albedo.a * MaterialData3[2];
    if (alpha < 0.5f)
        discard;
    float depth = pin.depth.x / pin.depth.y;
    return float4(depth, depth, depth, 1.0f);
}