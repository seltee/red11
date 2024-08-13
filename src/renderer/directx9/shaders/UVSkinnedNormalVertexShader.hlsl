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
    float3 shadowCoord[3] : TEXCOORD5;
};

matrix ViewProj : register(c0);

matrix LightsShadowMatricies[4] : register(c16);

// 0 - 12 - matricies
// 16 - 32 - light shadow matricies
// 32 - 256 - bones data = 56 bones

matrix BoneMatrices[56] : register(c32);

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
    for (int p = 0; p < 3; p++)
    {
        float4 shadowCoord = mul(float4(vout.worldPos, 1.0), LightsShadowMatricies[p]);
        shadowCoord.xyz /= shadowCoord.w;
        shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
        shadowCoord.y = 1.0 - shadowCoord.y;
        vout.shadowCoord[p] = shadowCoord.xyz;
    }
    return vout;
}