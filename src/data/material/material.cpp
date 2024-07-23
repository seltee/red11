// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "material.h"

Material::Material()
{
    static unsigned int lastIndex = 0;

    index = lastIndex;
    lastIndex++;
    updIndex = 0;
}

Material::~Material()
{
}