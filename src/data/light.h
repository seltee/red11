// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "utils/utils.h"
#include "texture.h"

class Entity;

enum class LightShadowQuality
{
    Low,
    Medium,
    High,
    Ultra,
    Maximum,
    AmountOfValues
};

enum class LightType
{
    Directional,
    Spot,
    Omni,
    AmountOfValues
};

struct Attenuation
{
    float att0;
    float att1;
    float att2;

    Attenuation()
    {
        this->att0 = 0.1f;
        this->att1 = 0.125f;
        this->att2 = 0.2f;
    }

    Attenuation(float att1)
    {
        this->att0 = 0.0f;
        this->att1 = att1;
        this->att2 = 0.0f;
    }

    Attenuation(float att0, float att1, float att2)
    {
        this->att0 = att0;
        this->att1 = att1;
        this->att2 = att2;
    }
};

class Light
{
public:
    EXPORT Light();
    EXPORT Light(
        Vector3 &directionalNormal,
        Color &directionalColor,
        bool bShadowEnabled = false,
        LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT Light(
        Attenuation &omniAttenuation,
        float omniRadius,
        Color &omniColor,
        bool bShadowEnabled = false,
        LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT Light(Vector3 &spotDirection,
                 Attenuation &spotAttenuation,
                 float spotOuterRadius,
                 float spotInnerRadius,
                 float spotRange,
                 float spotFalloff,
                 Color &spotColor,
                 bool bShadowEnabled = false,
                 LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT bool isAffecting(Vector3 point, float radius);

    EXPORT void rebuildShadowTextures();

    EXPORT void transform(Entity *entity);

    EXPORT void setShadowState(bool bShadowEnabled, LightShadowQuality shadowQuality);

    inline LightType getType() { return type; }
    inline Vector3 &getPosition() { return position; }
    inline Vector3 &getNormal() { return normal; }
    inline Attenuation &getAttenuation() { return attenuation; }
    inline float getRadius() { return radius; }
    inline float getInnerRadius() { return innerRadius; }
    inline float getRange() { return range; }
    inline float getFalloff() { return falloff; }
    inline Color &getColor() { return color; }
    inline bool isShadowsEnabled() { return shadowTextures && bShadowEnabled; }
    inline Texture *getShadowTexture(int number) { return (shadowTextures && number >= 0 && number < numOfCascades) ? shadowTextures[number] : nullptr; }
    inline float getShadowTextureTexelSize() { return texelSize; }
    inline int getNumOfCascades() { return numOfCascades; }
    inline float getCascadeDistance() { return cascadeDistance; }

    inline void setShadowViewProjectionMatrix(Matrix4 mShadowViewProjection) { this->mShadowViewProjection = mShadowViewProjection; }
    inline Matrix4 getShadowViewProjectionMatrix() { return mShadowViewProjection; }

protected:
    LightType type;

    bool bShadowEnabled = false;
    LightShadowQuality shadowQuality;
    Texture **shadowTextures = nullptr;
    int numOfCascades = 0;
    int bufferSize = 0;
    float texelSize = 0.0f;
    float cascadeDistance = 1.0f;

    Vector3 originalNormal;

    Vector3 position;
    Vector3 normal;

    Attenuation attenuation;
    Color color;

    Matrix4 mShadowViewProjection;

    float radius;
    float innerRadius;
    float range;
    float falloff;
};