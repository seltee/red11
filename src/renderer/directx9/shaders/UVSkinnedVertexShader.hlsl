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
    float3 worldPos : TEXCOORD2;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD;
    float3 shadowCoord[2] : TEXCOORD3;
};

matrix ViewProj : register(c0);

matrix LightsShadowMatricies[2] : register(c52);

// 0 - 16 - shader data
// 20 - 60 - light data
// 60 - 256 - bones data = 49 bones

matrix BoneMatrices[49] : register(c60);

VS_Output main(VS_Input vin)
{
    float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 normal = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        int boneIndex = int(vin.boneIndices[i]);
        float weight = vin.boneWeights[i];

        float4x4 world = BoneMatrices[boneIndex];

        position += weight * mul(float4(vin.pos, 1.0f), world);
        normal += weight * mul(vin.normal, (float3x3)world);
    }

    VS_Output vout;

    vout.pos = mul(position, ViewProj);
    vout.worldPos = position;
    vout.normal = normalize(normal);
    vout.uv = vin.uv;

    for (int i = 0; i < 2; i++)
    {
        float4 shadowCoord = mul(float4(vout.worldPos, 1.0), LightsShadowMatricies[i]);
        shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
        shadowCoord.y = 1.0 - shadowCoord.y;
        vout.shadowCoord[i] = shadowCoord.xyz;
    }

    return vout;
}