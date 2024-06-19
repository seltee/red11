// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "materialSimple.h"

MaterialSimple::MaterialSimple()
{
}

MaterialSimple::MaterialSimple(Color defuseColor, Color emissionColor)
{
    this->defuseColor = defuseColor;
    this->emissionColor = emissionColor;
}

MaterialSimple::MaterialSimple(Texture *defuseTexture, Texture *emissionTexture)
{
    this->defuseTexture = defuseTexture;
    this->emissionTexture = emissionTexture;

    this->defuseColor = Color(1.0f, 1.0f, 1.0f);
}

MaterialType MaterialSimple::getType()
{
    return MaterialType::Simple;
}
