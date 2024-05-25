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
    EXPORT void setupDirectional(Vector3 directionalNormal, Color directionalColor);
    EXPORT void setupOmni(Vector3 ommiPosition, Attenuation omniAttenuation, float omniRange, Color omniColor);
    EXPORT void setupSpot(Vector3 spotPosition,
                          Vector3 spotDirection,
                          Attenuation spotAttenuation,
                          float spotInnerRadius,
                          float spotOuterRadius,
                          float spotRange,
                          float spotFalloff,
                          Color spotColor);

    EXPORT void onRender(Renderer *renderer) override final;
    EXPORT void onRenderQueue(Renderer *renderer) override final;

    inline void setEnabled(bool bState) { bEnabled = bState; }
    inline bool getEnabledState() { return bEnabled; }

protected:
    Light light;
    Light worldLight;

    bool bEnabled = true;
};