// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "light.h"
#include "data/entity.h"
#include "string.h"

Light::Light()
{
    this->type = LightType::Directional;
    this->normal = Vector3(0.0f, 0.0f, -1.0f);
    this->attenuation = Attenuation();
    this->position = Vector3(0.0f, 0.0f, 0.0f);
    this->color = Color(0.8f, 0.8f, 0.8f);
    this->range = 1.0f;
    this->radius = 1.0f;
    this->innerRadius = 0.0f;
    this->falloff = 1.0f;
}

Light::Light(Vector3 &directionalNormal, Color &directionalColor)
{
    this->type = LightType::Directional;
    this->normal = directionalNormal;
    this->attenuation = Attenuation();
    this->position = Vector3(0.0f, 0.0f, 0.0f);
    this->color = directionalColor;
    this->range = 1.0f;
    this->radius = 1.0f;
    this->innerRadius = 0.0f;
    this->falloff = 1.0f;
}

Light::Light(Vector3 &omniPosition, Attenuation &omniAttenuation, float omniRange, Color &omniColor)
{
    this->type = LightType::Omni;
    this->normal = Vector3(0.0f, 0.0f, -1.0f);
    this->attenuation = omniAttenuation;
    this->position = omniPosition;
    this->color = omniColor;
    this->range = omniRange;
    this->radius = 0.0f;
    this->innerRadius = 0.0f;
    this->falloff = 1.0f;
}

Light::Light(Vector3 &spotPosition,
             Vector3 &spotDirection,
             Attenuation &spotAttenuation,
             float spotOuterRadius,
             float spotInnerRadius,
             float spotRange,
             float spotFalloff,
             Color &spotColor)
{
    this->type = LightType::Spot;
    this->normal = spotDirection;
    this->attenuation = spotAttenuation;
    this->position = spotPosition;
    this->color = spotColor;
    this->radius = spotOuterRadius;
    this->innerRadius = spotInnerRadius;
    this->range = spotRange;
    this->falloff = spotFalloff;
}

Light Light::transform(Entity *entity)
{
    Matrix4 *m = entity->getModelMatrix();

    if (type == LightType::Directional)
    {
        Quat rotation = glm::quat_cast(*m);
        Vector3 newNormal = glm::normalize(Vector3(rotation * Vector4(normal, 1.0f)));
        return Light(newNormal, color);
    }

    if (type == LightType::Omni)
    {
        Vector3 newPosition = *m * Vector4(position, 1.0f);
        return Light(newPosition, attenuation, range, color);
    }

    if (type == LightType::Spot)
    {
        Vector3 newPosition = *m * Vector4(position, 1.0f);
        Quat rotation = glm::quat_cast(*m);
        Vector3 newNormal = glm::normalize(Vector3(rotation * Vector4(normal, 1.0f)));
        return Light(newPosition, newNormal, attenuation, radius, innerRadius, range, falloff, color);
    }

    return Light(normal, color);
}