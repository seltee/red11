// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "utils/utils.h"

class Entity;

enum class LightType
{
    Directional,
    Spot,
    Omni
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
    EXPORT Light(Vector3 &directionalNormal, Color &directionalColor);
    EXPORT Light(Vector3 &omniPosition, Attenuation &omniAttenuation, float omniRadius, Color &omniColor);
    EXPORT Light(Vector3 &spotPosition,
                 Vector3 &spotDirection,
                 Attenuation &spotAttenuation,
                 float spotOuterRadius,
                 float spotInnerRadius,
                 float spotRange,
                 float spotFalloff,
                 Color &spotColor);

    EXPORT bool isAffecting(Vector3 point, float radius);

    EXPORT Light transform(Entity *entity);

    inline LightType getType() { return type; }
    inline Vector3 &getPosition() { return position; }
    inline Vector3 &getNormal() { return normal; }
    inline Attenuation &getAttenuation() { return attenuation; }
    inline float getRadius() { return radius; }
    inline float getInnerRadius() { return innerRadius; }
    inline float getRange() { return range; }
    inline float getFalloff() { return falloff; }
    inline Color &getColor() { return color; }

protected:
    LightType type;

    Vector3 position;
    Vector3 normal;
    Attenuation attenuation;
    Color color;
    float radius;
    float innerRadius;
    float range;
    float falloff;
};