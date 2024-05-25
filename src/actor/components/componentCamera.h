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

protected:
    Camera camera;
};