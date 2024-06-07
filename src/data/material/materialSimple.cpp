// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "materialSimple.h"

MaterialSimple::MaterialSimple()
{
}

MaterialSimple::MaterialSimple(Color defuseColor, Color emissionColor)
{
    MaterialSimple(defuseColor, emissionColor, defuseColor);
}

MaterialSimple::MaterialSimple(Color defuseColor, Color emissionColor, Color ambientColor)
{
    this->defuseColor = defuseColor;
    this->emissionColor = emissionColor;
    this->ambientColor = ambientColor;
}

MaterialSimple::MaterialSimple(Texture *defuseTexture, Texture *emissionTexture, Texture *ambientTexture)
{
    this->defuseTexture = defuseTexture;
    this->emissionTexture = emissionTexture;
    this->ambientTexture = ambientTexture;

    this->defuseColor = Color(1.0f, 1.0f, 1.0f);
    this->ambientColor = Color(1.0f, 1.0f, 1.0f);
    this->emissionColor = Color(1.0f, 1.0f, 1.0f);
}

MaterialType MaterialSimple::getType()
{
    return MaterialType::Simple;
}
