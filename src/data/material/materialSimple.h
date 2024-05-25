// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "material.h"
#include "utils/utils.h"
#include "utils/primitives.h"

class MaterialSimple : public Material
{
public:
    EXPORT MaterialSimple();
    EXPORT MaterialSimple(Color defuse);
    EXPORT MaterialSimple(Color defuse, Color emission);
    EXPORT MaterialSimple(Color defuse, Color emission, Color ambient);

    EXPORT MaterialType getType() override;

    Color defuse;
    Color ambient;
    Color emission;
};