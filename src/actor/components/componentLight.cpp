// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentLight.h"

void ComponentLight::setupDirectional(Vector3 directionalNormal, Color directionalColor, bool bEnableShadow, LightShadowQuality shadowQuality)
{
    if (this->light)
        delete this->light;

    this->light = new Light(directionalNormal, directionalColor, bEnableShadow, shadowQuality);
}

void ComponentLight::setupOmni(Attenuation omniAttenuation,
                               float omniRange,
                               Color omniColor,
                               bool bEnableShadow,
                               LightShadowQuality shadowQuality)
{
    if (this->light)
        delete this->light;

    this->light = new Light(omniAttenuation, omniRange, omniColor, bEnableShadow, shadowQuality);
}

void ComponentLight::setupSpot(Vector3 spotDirection,
                               Attenuation spotAttenuation,
                               float spotInnerRadius,
                               float spotOuterRadius,
                               float spotRange,
                               float spotFalloff,
                               Color spotColor,
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
        spotRange,
        spotFalloff,
        spotColor,
        bEnableShadow,
        shadowQuality);
}

void ComponentLight::onRenderQueue(Renderer *renderer)
{
    if (bEnabled && light)
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