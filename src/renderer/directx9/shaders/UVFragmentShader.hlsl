sampler2D albedoTexSampler : register(s0);
sampler2D alphaTexSampler : register(s1);
sampler2D emissionTexSampler : register(s2);
sampler2D normalTexSampler : register(s3);
sampler2D metallicTexSampler : register(s4);
sampler2D roughnessTexSampler : register(s5);
sampler2D aoTexSampler : register(s6);

sampler2D shadowTexSampler[8] : register(s8);

struct VS_Output
{
    float4 pos : POSITION;
    float3 worldPos : TEXCOORD2;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD;
    float3 shadowCoord[2] : TEXCOORD3;
};

struct Light
{
    float4 type;     // 1 - directional, 2 - castShadow, 3 - texel size
    float4 position; // x, y, z
    float4 normal;   // x, y, z
    float4 color;    // r, g, b
};

// useAlbedoTexture, useAlphaTexture, useNormalTexture, useMetallicTexture;
float4 MaterialData1 : register(c12);
// useRoughnessTexture, useAOTexture, useEmissionTexture;
float4 MaterialData2 : register(c13);
// roughnessValue, metallicValue, alphaValue, discardLowAlpha;
float4 MaterialData3 : register(c14);

float4 AlbedoColor : register(c15);
float4 EmissionColor : register(c16);
float4 CameraPosition : register(c17);
float4 AmbientLightColor : register(c18);

Light Lights[8] : register(c20);

float ShadowCalculation(sampler2D shadowTexSampler, float3 fragPosLightSpace, float3 lightDir, float3 Normal, float texelSize)
{
    float3 projCoords = fragPosLightSpace;
    float currentDepth = projCoords.z;
    float bias = max(0.002 * (1.0 - dot(Normal, lightDir)), 0.0005);
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 coords = projCoords.xy + float2(x, y) * texelSize;
            float pcfDepth = tex2D(shadowTexSampler, coords).r;
            shadow += step(step(currentDepth - bias, pcfDepth) + currentDepth, 1.0);
        }
    }
    if (shadow < 0.09)
        return 0;
    shadow /= 9.0;
    return shadow;
}

float4 main(VS_Output pin) : SV_TARGET
{
    // Simplified material properties
    float4 albedo = MaterialData1[0] == 1.0f ? tex2D(albedoTexSampler, pin.texCoord) * AlbedoColor : AlbedoColor;
    float alpha = MaterialData1[1] == 1.0f ? tex2D(alphaTexSampler, pin.texCoord).a * MaterialData3[2] : albedo.a * MaterialData3[2];
    if (MaterialData3[3] == 1.0f && alpha < 0.5f)
        discard;
    float3 emission = MaterialData2[2] == 1.0f ? (float3)tex2D(emissionTexSampler, pin.texCoord) * (float3)EmissionColor : (float3)EmissionColor;
    float metallic = MaterialData1[3] == 1.0f ? tex2D(metallicTexSampler, pin.texCoord).r * MaterialData3[1] : MaterialData3[1];
    float roughness = max(MaterialData2[0] == 1.0f ? tex2D(roughnessTexSampler, pin.texCoord).r * MaterialData3[0] : MaterialData3[0], 0.0001);
    float ao = MaterialData2[1] ? tex2D(aoTexSampler, pin.texCoord).r : 1.0;

    float3 N = normalize(pin.normal);
    float3 V = normalize((float3)CameraPosition - pin.worldPos);
    float3 H = float3(0.0, 0.0, 0.0);
    float3 L = float3(0.0, 0.0, 0.0);
    float3 diffuse = (float3)albedo / 3.14159265359;

    float3 color = (float3)albedo * (float3)AmbientLightColor * ao + emission;

    for (int i = 0; i < 8; i++)
    {
        if (Lights[i].type[0] == 0.0f)
            break;
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

            float shadow = 1.0f;
            if (Lights[i].type[1] == 1.0f)
            {
                // shadow enabled
                shadow = 1.0 - ShadowCalculation(shadowTexSampler[i * 2], pin.shadowCoord[i], L, N, Lights[i].type[2]);
            }

            // Accumulate lighting contributions
            color += (kD * diffuse + specular) * (float3)Lights[i].color * NdotL * shadow;
        }
    }

    // gamma
    color = pow(color, 1.0 / 1.2);

    return float4(color, alpha);
}