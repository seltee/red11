// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "utils/utils.h"

#define MAX_MATERIAL_COUNT 10000

enum class MaterialType
{
    Simple,
    Complex
};

enum class MaterialDisplay
{
    Solid,
    SolidMask,
    Alpha
};

class Material
{
public:
    EXPORT Material();
    EXPORT virtual ~Material();
    virtual MaterialType getType() = 0;
    virtual MaterialDisplay getDisplay() = 0;
    virtual bool isUsingNormalMap() = 0;
    virtual bool isAlphaPhase() = 0;

    inline unsigned int getIndex() { return index; }
    inline unsigned int getUpdateIndex() { return updIndex; }

protected:
    unsigned int index;
    unsigned int updIndex;
};
