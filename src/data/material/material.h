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

class Material
{
public:
    EXPORT Material();
    virtual MaterialType getType() = 0;
    inline unsigned int getIndex() { return index; }

protected:
    unsigned int index;
};
