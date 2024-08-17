// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "component.h"
#include "data/spline.h"
#include "utils/utils.h"

class ComponentSpline : public Component
{
public:
    EXPORT ComponentSpline();
    EXPORT ~ComponentSpline();

    EXPORT void onRenderQueue(Renderer *renderer) override final;

    EXPORT void setSpline(Spline *spline);

    inline Vector3 getPointOnSpline(float fDistance)
    {
        if (spline)
            return Vector3(getModelMatrix() * Vector4(spline->getPointOnSpline(fDistance), 1.0f));
        return Vector3(0.0f);
    }

    inline Spline *getSpline() { return spline; }

    inline void showDebugSpline(bool state) { bShowDebugSpline = state; };

protected:
    Spline *spline = nullptr;
    bool bShowDebugSpline = false;
};