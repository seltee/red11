// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "light.h"
#include "data/entity.h"
#include "string.h"

Light::Light()
{
    this->type = LightType::Directional;
    this->normal = Vector3(0.0f, 0.0f, -1.0f);
    this->originalNormal = Vector3(0.0f, 0.0f, -1.0f);
    this->attenuation = Attenuation();
    this->position = Vector3(0.0f, 0.0f, 0.0f);
    this->color = Color(0.8f, 0.8f, 0.8f);
    this->radius = 1.0f;
    this->innerRadius = 0.0f;
    this->falloff = 1.0f;

    rebuildShadowTextures();
}

Light::Light(Vector3 &directionalNormal, Color &directionalColor, bool bShadowEnabled, LightShadowQuality shadowQuality)
{
    this->type = LightType::Directional;
    this->normal = glm::normalize(directionalNormal);
    this->originalNormal = this->normal;
    this->attenuation = Attenuation();
    this->position = Vector3(0.0f, 0.0f, 0.0f);
    this->color = directionalColor;
    this->radius = 1.0f;
    this->innerRadius = 0.0f;
    this->falloff = 1.0f;
    this->bShadowEnabled = bShadowEnabled;
    this->shadowQuality = shadowQuality;

    rebuildShadowTextures();
}

Light::Light(Attenuation &omniAttenuation, Color &omniColor, bool bShadowEnabled, LightShadowQuality shadowQuality)
{
    this->type = LightType::Omni;
    this->normal = Vector3(0.0f, 0.0f, -1.0f);
    this->originalNormal = this->normal;
    this->attenuation = omniAttenuation;
    this->position = Vector3(0.0f, 0.0f, 0.0f);
    this->color = omniColor;
    this->radius = omniAttenuation.calcRadius();
    this->innerRadius = 0.0f;
    this->falloff = 1.0f;
    this->bShadowEnabled = bShadowEnabled;
    this->shadowQuality = shadowQuality;

    rebuildShadowTextures();
}

Light::Light(Vector3 &spotDirection,
             Attenuation &spotAttenuation,
             float spotOuterRadius,
             float spotInnerRadius,
             float spotFalloff,
             Color &spotColor,
             bool bShadowEnabled,
             LightShadowQuality shadowQuality)
{
    this->type = LightType::Spot;
    this->normal = glm::normalize(spotDirection);
    this->originalNormal = this->normal;
    this->attenuation = spotAttenuation;
    this->position = Vector3(0.0f, 0.0f, 0.0f);
    this->color = spotColor;
    this->radius = spotOuterRadius;
    this->innerRadius = spotInnerRadius;
    this->falloff = spotFalloff;
    this->bShadowEnabled = bShadowEnabled;
    this->shadowQuality = shadowQuality;

    rebuildShadowTextures();
}

float Light::isAffecting(Vector3 point, float radius)
{
    if (type == LightType::Directional)
        return 0.00001f;
    if (type == LightType::Omni)
    {
        float distance = glm::distance(position, point);
        // if (distance < radius + this->radius)
        return distance + 0.000011f; // to make it always more far than directional lights
        // return 0.0f;
    }
    return 0.0f;
}

void Light::rebuildShadowTextures()
{
    if (shadowTextures)
    {
        for (int i = 0; i < numOfCascades; i++)
            shadowTextures[i]->destroy();
        shadowTextures = nullptr;
        numOfCascades = 0;
    }

    if (!bShadowEnabled)
        return;

    if (type == LightType::Directional)
    {
        switch (shadowQuality)
        {
        case LightShadowQuality::Low:
            numOfCascades = 1;
            bufferSize = 1024;
            cascadeDistance = 2.4f;
            break;
        case LightShadowQuality::Medium:
            bufferSize = 1024;
            numOfCascades = 1;
            cascadeDistance = 2.4f;
            break;
        case LightShadowQuality::High:
            bufferSize = 2048;
            numOfCascades = 1;
            cascadeDistance = 3.2f;
            break;
        case LightShadowQuality::Ultra:
            bufferSize = 2048;
            numOfCascades = 1;
            cascadeDistance = 3.2f;
            break;
        case LightShadowQuality::AmountOfValues:
        case LightShadowQuality::Maximum:
            bufferSize = 4096;
            numOfCascades = 1;
            cascadeDistance = 4.4f;
            break;
        }
    }

    if (type == LightType::Omni)
    {
        numOfCascades = 6;
        switch (shadowQuality)
        {
        case LightShadowQuality::Low:
            bufferSize = 256;
            break;
        case LightShadowQuality::Medium:
            bufferSize = 512;
            break;
        case LightShadowQuality::High:
            bufferSize = 1024;
            break;
        case LightShadowQuality::Ultra:
            bufferSize = 2048;
            break;
        case LightShadowQuality::AmountOfValues:
        case LightShadowQuality::Maximum:
            bufferSize = 4096;
            break;
        }
    }

    if (type == LightType::Spot)
        return;

    texelSize = 1.0f / ((float)bufferSize);

    shadowTextures = new Texture *[numOfCascades];
    for (int i = 0; i < numOfCascades; i++)
    {
        shadowTextures[i] = new Texture("LightShadowCascade_" + std::to_string(numOfCascades), TextureType::GpuStencil);
        shadowTextures[i]->setGpuRenderSize(bufferSize, bufferSize);
    }
}

void Light::transform(Entity *entity)
{
    Matrix4 *m = entity->getModelMatrix();

    if (type == LightType::Directional)
    {
        Quat rotation = glm::quat_cast(*m);
        normal = glm::normalize(Vector3(rotation * Vector4(originalNormal, 1.0f)));
    }

    if (type == LightType::Omni)
    {
        position = Vector3(*m * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    if (type == LightType::Spot)
    {
        position = Vector3(*m * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        Quat rotation = glm::quat_cast(*m);
        normal = glm::normalize(Vector3(rotation * Vector4(originalNormal, 1.0f)));
    }
}

void Light::setShadowState(bool bShadowEnabled, LightShadowQuality shadowQuality)
{
    this->bShadowEnabled = bShadowEnabled;
    this->shadowQuality = shadowQuality;
    rebuildShadowTextures();
}
