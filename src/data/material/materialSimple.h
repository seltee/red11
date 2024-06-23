// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "material.h"
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/texture.h"

class MaterialSimple : public Material
{
public:
    EXPORT MaterialSimple();
    EXPORT MaterialSimple(
        Color albedoColor,
        Color emissionColor = Color(0.0f, 0.0f, 0.0f),
        float metallic = 0.0f,
        float roughness = 0.5f);

    EXPORT MaterialSimple(
        Texture *albedoTexture,
        Texture *normalTexture = nullptr,
        Texture *emissionTexture = nullptr,
        Texture *metallicTexture = nullptr,
        Texture *roughnessTexture = nullptr,
        Texture *AOTexture = nullptr);

    EXPORT MaterialType getType() override;
    EXPORT bool isUsingNormalMap() override;

    inline Color &getAlbedoColor() { return albedoColor; }
    inline Color &getEmissionColor() { return emissionColor; }
    inline float getMetallic() { return metallic; }
    inline float getRoughness() { return roughness; }

    EXPORT void setAlbedoColor(Color albedoColor);
    EXPORT void setEmissionColor(Color emissionColor);
    EXPORT void setMetallic(float metallic);
    EXPORT void setRoughness(float roughness);

    inline Texture *getAlbedoTexture() { return albedoTexture; }
    inline Texture *getEmissionTexture() { return emissionTexture; }
    inline Texture *getNormalTexture() { return normalTexture; }
    inline Texture *getMetallicTexture() { return metallicTexture; }
    inline Texture *getRoughnessTexture() { return roughnessTexture; }
    inline Texture *getAOTexture() { return AOTexture; }

    EXPORT void setAlbedoTexture(Texture *texture);
    EXPORT void setEmissionTexture(Texture *texture);
    EXPORT void setNormalTexture(Texture *texture);
    EXPORT void setMetallicTexture(Texture *texture);
    EXPORT void setRoughnessTexture(Texture *texture);
    EXPORT void setAOTexture(Texture *texture);

protected:
    Color albedoColor = Color(0.64f, 0.64f, 0.46f);
    Color emissionColor = Color(0.0f, 0.0f, 0.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;

    Texture *albedoTexture = nullptr;
    Texture *emissionTexture = nullptr;
    Texture *normalTexture = nullptr;
    Texture *metallicTexture = nullptr;
    Texture *roughnessTexture = nullptr;
    Texture *AOTexture = nullptr;
};