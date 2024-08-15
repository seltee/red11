// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentLight.h"

void ComponentLight::setupDirectional(const Vector3 directionalNormal, const Color directionalColor, bool bEnableShadow, LightShadowQuality shadowQuality)
{
    if (this->light)
        delete this->light;

    this->light = new Light(directionalNormal, directionalColor, bEnableShadow, shadowQuality);
}

void ComponentLight::setupOmni(const Attenuation omniAttenuation,
                               const Color omniColor,
                               bool bEnableShadow,
                               LightShadowQuality shadowQuality)
{
    if (this->light)
        delete this->light;

    this->light = new Light(omniAttenuation, omniColor, bEnableShadow, shadowQuality);
}

void ComponentLight::setupSpot(const Vector3 spotDirection,
                               const Attenuation spotAttenuation,
                               float spotInnerRadius,
                               float spotOuterRadius,
                               const Color spotColor,
                               bool bEnableShadow,
                               LightShadowQuality shadowQuality)
{
    if (this->light)
        delete this->light;

    this->light = new Light(
        spotDirection,
        spotAttenuation,
        spotInnerRadius,
        spotOuterRadius,
        spotColor,
        bEnableShadow,
        shadowQuality);
}

void ComponentLight::onRenderQueue(Renderer *renderer)
{
    if (bVisible && light)
    {
        light->transform(this);
        renderer->queueLight(light);
    }
}

void ComponentLight::setShadowState(bool bShadowEnabled, LightShadowQuality shadowQuality)
{
    if (light)
        light->setShadowState(bShadowEnabled, shadowQuality);
}