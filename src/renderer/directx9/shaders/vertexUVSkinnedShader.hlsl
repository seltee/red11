struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : POSITION;
    float2 uv : TEXCOORD;
    float4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VS_Output
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

matrix ViewProj : register(c0);

// 0 - 16 - shader data
// 16 - 48 - light data
// 48 - 256 - bones data = 52 bones

matrix BoneMatrices[52] : register(c48);

VS_Output main(VS_Input vin)
{
    float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        int boneIndex = int(vin.boneIndices[i]);
        float weight = vin.boneWeights[i];
        position += weight * mul(float4(vin.pos, 1.0f), BoneMatrices[boneIndex]);
    }

    VS_Output vout;

    vout.pos = mul(position, ViewProj);
    vout.uv = vin.uv;

    return vout;
}