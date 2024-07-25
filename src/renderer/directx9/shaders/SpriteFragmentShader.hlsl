sampler2D textureSampler : register(s0);

struct VS_Output
{
    float4 pos : POSITION;
    float2 texCoord : TEXCOORD;
};

float4 color : register(c0);
// Use texture, As mask, *, *
float4 settings : register(c1);

float4 main(VS_Output pin) : SV_TARGET
{
    float4 outColor;
    if (settings[1] == 1.0)
    {
        // Mask
        outColor = color;
        if (settings[0] == 1.0)
        {
            // Use texture
            outColor.a *= tex2D(textureSampler, pin.texCoord).a;
        }
    }
    else
    {
        // Normal color
        if (settings[0] == 1.0)
        {
            // Use texture
            outColor = tex2D(textureSampler, pin.texCoord);
        }
        else
        {
            // Use color
            outColor = color;
        }
    }
    return outColor;
}
