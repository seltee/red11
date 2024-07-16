struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VS_Output
{
    float4 pos : POSITION;
    float2 depth : TEXCOORD1;
};

matrix ViewProj : register(c0);

// 0 - 16 - shader data
// 20 - 60 - light data
// 60 - 256 - bones data = 49 bones

matrix BoneMatrices[49] : register(c60);

VS_Output main(VS_Input vin)
{
    float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        int boneIndex = int(vin.boneIndices[i]);
        float weight = vin.boneWeights[i];

        float4x4 world = BoneMatrices[boneIndex];

        position += weight * mul(float4(vin.pos, 1.0f), world);
    }

    VS_Output vout;

    vout.pos = mul(position, ViewProj);
    // vout.pos.z *= vout.pos.w;
    // vout.pos.w = 1.0;
    vout.depth = float2(vout.pos.z, vout.pos.w);

    return vout;
}