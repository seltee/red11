struct Light
{
    float4 type;     // 1 - directional / omni / spot, 2 - castShadow, 3 - texel size / outer radius, 4 - constant
    float4 position; // x, y, z, 4 - linear
    float4 normal;   // x, y, z, 4 - quadratic
    float4 color;    // r, g, b, 4 - radius / inner radius
};

inline float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) * 0.125;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

inline float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    denom = 3.14159265359 * denom * denom;
    return a2 / denom;
}

inline float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

inline float ShadowCalculation(sampler2D shadowTexSampler, float3 fragPosLightSpace, float texelSize, float bias)
{
    float2 projCoords = fragPosLightSpace.xy;
    float currentDepth = fragPosLightSpace.z;
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 coords = (projCoords) + float2(x, y) * texelSize;
            float pcfDepth = tex2D(shadowTexSampler, coords).r;
            shadow += step(step(currentDepth - bias, pcfDepth) + currentDepth, 1.0);
        }
    }
    if (shadow < 0.09)
        return 0;
    shadow /= 9.0;
    float edgeX = min((0.5 - abs(projCoords.x - 0.5)) * 8.0, 1);
    float edgeY = min((0.5 - abs(projCoords.y - 0.5)) * 8.0, 1);
    float edge = min(edgeX, edgeY);
    return shadow * edge;
}

inline float ShadowCalculationL(sampler2D shadowTexSampler, float3 fragPosLightSpace, float bias)
{
    float depth = tex2D(shadowTexSampler, fragPosLightSpace.xy).r;
    return step(step(fragPosLightSpace.z - bias, depth) + fragPosLightSpace.z, 1.0);
}

inline float3 CaclLightWithShadow(
    Light light,
    sampler2D lightShadowTexSampler,
    sampler2D lightAdditionShadowTexSampler,
    float3 diffuse,
    float metallic,
    float roughness,
    float3 N,
    float3 V,
    float3 worldPos,
    float3 shadowCoords,
    float3 polygonNormal)
{
    float type = light.type[0];

    float3 H, L, F;
    float isShadowsEnabled = light.type[1];

    float attenuation = 1.0f;
    float shadow = 1.0f;

    // Direction
    if (type == 1.0f)
    {
        L = -light.normal.xyz;
    }
    // Omni / Spot
    if (type == 2.0f || type == 3.0f)
    {
        L = light.position.xyz - worldPos;
        float distance = length(L);
        L = normalize(L);
        attenuation = 1.0 / (light.type[3] +
                             light.position[3] * distance +
                             light.normal[3] * distance * distance);

        // Spot attenuation
        if (type == 3.0f)
        {
            float3 spotDirection = light.normal.xyz;
            float spotEffect = dot(L, spotDirection);
            float innerCone = light.color[3];
            float outerCone = light.type[2];
            float spotAttenuation = saturate((spotEffect - outerCone) / (innerCone - outerCone));
            // Final attenuation combining distance and spot attenuation
            attenuation *= spotAttenuation;
        }
    }

    H = normalize(V + L);
    F = float3(0.04, 0.04, 0.04) + (float3(0.96, 0.96, 0.96)) * pow(1.0 - max(dot(H, V), 0.0), 5.0);

    // Approximate GGX for DirectX9 limitations
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float3 kD = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic);

    // Simplified specular
    // float3 specular = F * pow(NdotH, 8.0 / roughness);

    float NDF = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(NdotV, 0.0) * max(NdotL, 0.0) + 0.0001;
    float3 specular = numerator / denominator;

    // If shadows are enabled
    if (isShadowsEnabled == 1.0f)
    {
        if (type == 1.0f)
        {
            shadowCoords.xy = shadowCoords.xy * 0.5 + 0.5;
            shadowCoords.y = 1.0 - shadowCoords.y;
            float bias = max(0.0002 * (1.0 - dot(N, L)), 0.00004);
            shadow = 1.0 - ShadowCalculation(lightShadowTexSampler, shadowCoords, light.type[2], bias);
        }
        if (type == 3.0f)
        {
            shadowCoords.xy = shadowCoords.xy + 0.5;
            shadowCoords.y = 1.0 - shadowCoords.y;
            float bias = max(0.04 * (1.0 - dot(N, L)), 0.0032);
            float mask = tex2D(lightAdditionShadowTexSampler, shadowCoords.xy).r;
            shadow = (1.0 - ShadowCalculationL(lightShadowTexSampler, shadowCoords, bias)) * mask;
        }
    }

    if (type > 0.1)
    {
        return (kD * diffuse + specular) * light.color.xyz * NdotL * attenuation * shadow;
    }
    else
    {
        return float3(0, 0, 0);
    }
}

inline float3 CaclLight(
    Light light,
    float3 diffuse,
    float metallic,
    float roughness,
    float3 N,
    float3 V,
    float3 worldPos)
{
    float type = light.type[0];

    float3 H, L, F;

    float attenuation = 1.0f;

    // Direction
    if (type == 1.0f)
    {
        L = -light.normal.xyz;
    }
    // Omni / Spot
    if (type == 2.0f || type == 3.0f)
    {
        L = light.position.xyz - worldPos;
        float distance = length(L);
        L = normalize(L);
        attenuation = 1.0 / (light.type[3] +
                             light.position[3] * distance +
                             light.normal[3] * distance * distance);

        // Spot attenuation
        if (type == 3.0f)
        {
            float3 spotDirection = light.normal.xyz;
            float spotEffect = dot(L, spotDirection);
            float innerCone = light.color[3];
            float outerCone = light.type[2];
            float spotAttenuation = saturate((spotEffect - outerCone) / (innerCone - outerCone));
            // Final attenuation combining distance and spot attenuation
            attenuation *= spotAttenuation;
        }
    }

    H = normalize(V + L);
    F = float3(0.04, 0.04, 0.04) + (float3(0.96, 0.96, 0.96)) * pow(1.0 - max(dot(H, V), 0.0), 5.0);

    // Approximate GGX for DirectX9 limitations
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float3 kD = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic);

    // Simplified specular
    // float3 specular = F * pow(NdotH, 8.0 / roughness);

    float NDF = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(NdotV, 0.0) * max(NdotL, 0.0) + 0.0001;
    float3 specular = numerator / denominator;

    return (kD * diffuse + specular) * light.color.xyz * NdotL * attenuation;
}