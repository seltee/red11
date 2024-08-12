// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/defines.h"

class Shader
{
public:
    virtual void use() = 0;
    virtual RendererType getType() = 0;
};
