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
    float constant;
    float linear;
    float quadratic;

    Attenuation()
    {
        this->constant = 1.0f;
        this->linear = 0.1f;
        this->quadratic = 0.01f;
    }

    Attenuation(float linear)
    {
        this->constant = 1.0f;
        this->linear = linear;
        this->quadratic = 0.01f;
    }

    Attenuation(float constant, float linear, float quadratic)
    {
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    inline float calcRadius() const
    {
        const float threshold = 0.01f;
        const float target = 1.0f / threshold;

        float discriminant = linear * linear - 4 * quadratic * (constant - target);

        if (discriminant < 0)
        {
            return 0.0f;
        }
        else
        {
            float R1 = (-linear + sqrt(discriminant)) / (2 * quadratic);
            float R2 = (-linear - sqrt(discriminant)) / (2 * quadratic);

            // Select the positive solution
            return fmaxf(R1, R2);
        }
    }
};

class Light
{
public:
    EXPORT Light();
    EXPORT Light(
        const Vector3 &directionalNormal,
        const Color &directionalColor,
        bool bShadowEnabled = false,
        LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT Light(
        const Attenuation &omniAttenuation,
        const Color &omniColor,
        bool bShadowEnabled = false,
        LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT Light(const Vector3 &spotDirection,
                 const Attenuation &spotAttenuation,
                 float spotOuterRadius,
                 float spotInnerRadius,
                 const Color &spotColor,
                 bool bShadowEnabled = false,
                 LightShadowQuality shadowQuality = LightShadowQuality::Low);

    // returns distance or 0.0f if not
    EXPORT float isAffecting(const Vector3 &point, float radius);

    EXPORT void rebuildShadowTextures();

    EXPORT void transform(Entity *entity);

    EXPORT void setShadowState(bool bShadowEnabled, LightShadowQuality shadowQuality);

    inline LightType getType() { return type; }
    inline Vector3 &getPosition() { return position; }
    inline Vector3 &getNormal() { return normal; }
    inline Attenuation &getAttenuation() { return attenuation; }
    inline float getRadius() { return radius; }
    inline float getInnerRadius() { return innerRadius; }
    inline float getAffectDistance() { return affectDistance; }
    inline Color &getColor() { return color; }
    inline bool isShadowsEnabled() { return shadowTextures && bShadowEnabled; }
    inline Texture *getShadowTexture(int number) { return (shadowTextures && number >= 0 && number < numOfCascades) ? shadowTextures[number] : nullptr; }
    inline float getShadowTextureTexelSize() { return texelSize; }
    inline int getNumOfCascades() { return numOfCascades; }
    inline float getCascadeDistance() { return cascadeDistance; }
    inline float getBufferSize() { return bufferSize; }
    inline void setShadowMaskTexture(Texture *texture) { this->shadowMaskTexture = texture; }
    inline Texture *getShadowMaskTexture() { return shadowMaskTexture; };

    inline void setShadowViewProjectionMatrix(int cascade, const Matrix4 &mShadowViewProjection) { this->mShadowViewProjection[cascade] = mShadowViewProjection; }
    inline Matrix4 getShadowViewProjectionMatrix(int cascade) { return mShadowViewProjection[cascade]; }

protected:
    LightType type;

    bool bShadowEnabled = false;
    LightShadowQuality shadowQuality;
    Texture **shadowTextures = nullptr;
    Texture *shadowMaskTexture = nullptr;
    int numOfCascades = 0;
    int bufferSize = 0;
    float texelSize = 0.0f;
    float cascadeDistance = 1.0f;

    Vector3 originalNormal;

    Vector3 position;
    Vector3 normal;

    Attenuation attenuation;
    Color color;

    Matrix4 mShadowViewProjection[2];

    float radius;
    float innerRadius;
    float affectDistance;
};