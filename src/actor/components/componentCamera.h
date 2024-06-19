// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/camera.h"
#include "component.h"

class ComponentCamera : public Component
{
public:
    EXPORT void onCreated() override;

    EXPORT void setupAsOrthographic(int width, int height, float nearDistance = -1400.0f, float farDistance = 1400.0f);
    EXPORT void setupAsPerspective(int width, int height, float nearDistance = 0.01f, float farDistance = 80.0f, float fov = 45.0f);

    EXPORT void onProcess(float delta) override final;

    inline Camera *getCamera() { return &camera; }

protected:
    Camera camera;
};