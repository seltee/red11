// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentLight.h"

void ComponentLight::setupDirectional(Vector3 directionalNormal, Color directionalColor)
{
    this->light = Light(directionalNormal, directionalColor);
}

void ComponentLight::setupOmni(Vector3 ommiPosition, Attenuation omniAttenuation, float omniRange, Color omniColor)
{
    this->light = Light(ommiPosition, omniAttenuation, omniRange, omniColor);
}

void ComponentLight::setupSpot(Vector3 spotPosition,
                               Vector3 spotDirection,
                               Attenuation spotAttenuation,
                               float spotInnerRadius,
                               float spotOuterRadius,
                               float spotRange,
                               float spotFalloff,
                               Color spotColor)
{
    this->light = Light(spotPosition, spotDirection, spotAttenuation, spotInnerRadius, spotOuterRadius, spotRange, spotFalloff, spotColor);
}

void ComponentLight::onRenderQueue(Renderer *renderer)
{
    if (bEnabled)
    {
        worldLight = light.transform(this);
        renderer->queueLight(&worldLight);
    }
}
