struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VS_Output
{
    float4 pos : POSITION;
    float3 worldPos : TEXCOORD2;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD;
    float3 tangent : TEXCOORD3;
    float3 bitangent : TEXCOORD4;
};

matrix ViewProj : register(c0);

// 0 - 16 - shader data
// 20 - 52 - light data
// 52 - 256 - bones data = 51 bones

matrix BoneMatrices[52] : register(c52);

VS_Output main(VS_Input vin)
{
    float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 normal = float3(0.0f, 0.0f, 0.0f);
    float3 tangent = float3(0.0f, 0.0f, 0.0f);
    float3 bitangent = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        int boneIndex = int(vin.boneIndices[i]);
        float weight = vin.boneWeights[i];

        float4x4 world = BoneMatrices[boneIndex];

        position += weight * mul(float4(vin.pos, 1.0f), world);
        normal += weight * mul(vin.normal, (float3x3)world);
        tangent += weight * mul(vin.tangent, (float3x3)world);
        bitangent += weight * mul(vin.bitangent, (float3x3)world);
    }

    VS_Output vout;

    vout.pos = mul(position, ViewProj);
    vout.worldPos = position;
    vout.normal = normalize(normal);
    vout.tangent = normalize(tangent);
    vout.bitangent = normalize(bitangent);
    vout.texCoord = vin.texCoord;

    return vout;
}