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
        float roughness = 0.5f,
        float alpha = 1.0f);

    EXPORT MaterialSimple(
        Texture *albedoTexture,
        Texture *normalTexture = nullptr,
        Texture *emissionTexture = nullptr,
        Texture *metallicTexture = nullptr,
        Texture *roughnessTexture = nullptr,
        Texture *AOTexture = nullptr);

    EXPORT MaterialSimple(
        MaterialDisplay display,
        Color albedoColor,
        Color emissionColor = Color(0.0f, 0.0f, 0.0f),
        float alpha = 1.0f,
        float metallic = 0.0f,
        float roughness = 0.5f);

    EXPORT MaterialSimple(
        MaterialDisplay display,
        float alpha = 1.0f,
        Texture *albedoTexture = nullptr,
        Texture *normalTexture = nullptr,
        Texture *emissionTexture = nullptr,
        Texture *metallicTexture = nullptr,
        Texture *roughnessTexture = nullptr,
        Texture *AOTexture = nullptr);

    EXPORT MaterialType getType() override;
    EXPORT MaterialDisplay getDisplay() override;
    EXPORT bool isUsingNormalMap() override;
    EXPORT bool isAlphaPhase() override;

    EXPORT bool isLoaded() override;
    EXPORT void load() override;
    EXPORT void unload() override;

    inline Color &getAlbedoColor() { return albedoColor; }
    inline Color &getEmissionColor() { return emissionColor; }
    inline float getMetallic() { return metallic; }
    inline float getRoughness() { return roughness; }
    inline float getAlpha() { return alpha; }
    inline MaterialDisplay getDisplayMode() { return display; }

    EXPORT void setDisplayMode(MaterialDisplay mode);
    EXPORT void setAlbedoColor(const Color &albedoColor);
    EXPORT void setEmissionColor(const Color &emissionColor);
    EXPORT void setMetallic(float metallic);
    EXPORT void setRoughness(float roughness);
    EXPORT void setAlpha(float alpha);

    inline Texture *getAlbedoTexture() { return albedoTexture; }
    inline Texture *getAlphaTexture() { return alphaTexture; }
    inline Texture *getEmissionTexture() { return emissionTexture; }
    inline Texture *getNormalTexture() { return normalTexture; }
    inline Texture *getMetallicTexture() { return metallicTexture; }
    inline Texture *getRoughnessTexture() { return roughnessTexture; }
    inline Texture *getAOTexture() { return AOTexture; }

    EXPORT void setAlbedoTexture(Texture *texture);
    EXPORT void setAlphaTexture(Texture *texture);
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
    float alpha = 1.0f;

    Texture *albedoTexture = nullptr;
    Texture *alphaTexture = nullptr;
    Texture *emissionTexture = nullptr;
    Texture *normalTexture = nullptr;
    Texture *metallicTexture = nullptr;
    Texture *roughnessTexture = nullptr;
    Texture *AOTexture = nullptr;

    MaterialDisplay display = MaterialDisplay::Solid;

    bool loaded = false;
};