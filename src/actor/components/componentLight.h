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
        const Vector3 directionalNormal,
        const Color directionalColor,
        bool bEnableShadow = false,
        LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT void setupOmni(const Attenuation omniAttenuation,
                          const Color omniColor,
                          bool bEnableShadow = false,
                          LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT void setupSpot(const Vector3 spotDirection,
                          const Attenuation spotAttenuation,
                          float spotInnerRadius,
                          float spotOuterRadius,
                          const Color spotColor,
                          bool bEnableShadow = false,
                          LightShadowQuality shadowQuality = LightShadowQuality::Low);

    EXPORT void onRenderQueue(Renderer *renderer) override final;

    EXPORT void setShadowState(bool bShadowEnabled, LightShadowQuality shadowQuality);

    inline Light *getLight() { return light; }

protected:
    Light *light = nullptr;
};