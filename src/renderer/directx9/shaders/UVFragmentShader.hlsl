// Albedo Texture
sampler2D albedoTexSampler : register(s0) =
                                 sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// Emission Texture
sampler2D emissionTexSampler : register(s1) =
                                   sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// Normal Texture
sampler2D normalTexSampler : register(s2) =
                                 sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// Metallic Texture
sampler2D metallicTexSampler : register(s3) =
                                   sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// Roughness Texture
sampler2D roughnessTexSampler : register(s5) =
                                    sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

// AO Texture
sampler2D aoTexSampler : register(s6) =
                             sampler_state
{
    Texture = <gTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

struct VS_Output
{
    float4 pos : POSITION;
    float3 worldPos : TEXCOORD2;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD;
};

struct Light
{
    float4 type;     // 1 - directional
    float4 position; // x, y, z
    float4 normal;   // x, y, z
    float4 color;    // r, g, b
};

// useAlbedoTexture, useNormalTexture, useMetallicTexture, useRoughnessTexture;
float4 MaterialData1 : register(c12);
// useAOTexture, useEmissionTexture, roughnessValue, metallicValue;
float4 MaterialData2 : register(c13);

float4 AmbientLightColor : register(c14);
float4 AlbedoColor : register(c15);
float4 EmissionColor : register(c16);
float4 CameraPosition : register(c17);

Light Lights[8] : register(c20);

float4 main(VS_Output pin) : SV_TARGET
{
    // Simplified material properties
    float3 albedo = MaterialData1[0] == 1.0f ? (float3)tex2D(albedoTexSampler, pin.uv) * (float3)AlbedoColor : (float3)AlbedoColor;
    float3 emission = MaterialData2[1] == 1.0f ? (float3)tex2D(emissionTexSampler, pin.uv) * (float3)EmissionColor : (float3)EmissionColor;
    float metallic = MaterialData1[2] == 1.0f ? tex2D(metallicTexSampler, pin.uv).r * MaterialData2[3] : MaterialData2[3];
    float roughness = max(MaterialData1[3] == 1.0f ? tex2D(roughnessTexSampler, pin.uv).r * MaterialData2[2] : MaterialData2[2], 0.0001);
    float ao = MaterialData2[0] ? tex2D(aoTexSampler, pin.uv).r : 1.0;

    float3 N = normalize(pin.normal);
    float3 V = normalize((float3)CameraPosition - pin.worldPos);
    float3 H = float3(0.0, 0.0, 0.0);
    float3 L = float3(0.0, 0.0, 0.0);
    float3 diffuse = albedo / 3.14159265359;

    float3 color = albedo * (float3)AmbientLightColor * ao + emission;

    for (int i = 0; i < 8; i++)
    {
        if (Lights[i].type[0] == 1.0f)
        {
            // Directional light direction is usually opposite
            L = -Lights[i].normal.xyz;
            H = normalize(V + L);

            float3 F = float3(0.04, 0.04, 0.04) + (float3(0.96, 0.96, 0.96)) * pow(1.0 - max(dot(H, V), 0.0), 5.0);

            // Approximate GGX for DirectX9 limitations
            float NdotL = max(dot(N, L), 0.0);
            float NdotH = max(dot(N, H), 0.0);
            float3 kD = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic);

            // Simplified specular
            float3 specular = F * pow(NdotH, 8.0 / roughness);

            // Accumulate lighting contributions
            color += (kD * diffuse + specular) * (float3)Lights[i].color * NdotL;
        }
    }

    // gamma
    color = pow(color, 1.0 / 1.2);

    return float4(color, 1.0f);
}