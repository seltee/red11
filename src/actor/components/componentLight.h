// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/light.h"
#include "data/entity.h"
#include "actor/components/component.h"

class ComponentLight : public Component
{
public:
    EXPORT void setupDirectional(
        Vector3 directionalNormal,
        Color directionalColor,
        bool bEnableShadow = false,
        LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT void setupOmni(Attenuation omniAttenuation,
                          Color omniColor,
                          bool bEnableShadow = false,
                          LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT void setupSpot(Vector3 spotDirection,
                          Attenuation spotAttenuation,
                          float spotInnerRadius,
                          float spotOuterRadius,
                          Color spotColor,
                          bool bEnableShadow = false,
                          LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT void onRenderQueue(Renderer *renderer) override final;

    EXPORT void setShadowState(bool bShadowEnabled, LightShadowQuality shadowQuality);

    inline Light *getLight() { return light; }

protected:
    Light *light = nullptr;
};