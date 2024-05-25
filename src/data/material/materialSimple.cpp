// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "materialSimple.h"

MaterialSimple::MaterialSimple()
{
    this->defuse = Color(0.16f, 0.16f, 0.16f);
    this->emission = Color(0.0f, 0.0f, 0.0f);
    this->ambient = this->defuse;
}

MaterialSimple::MaterialSimple(Color defuse)
{
    this->defuse = defuse;
    this->emission = Color(0.0f, 0.0f, 0.0f);
    this->ambient = defuse;
}

MaterialSimple::MaterialSimple(Color defuse, Color emission)
{
    this->defuse = defuse;
    this->emission = emission;
    this->ambient = defuse;
}

MaterialSimple::MaterialSimple(Color defuse, Color emission, Color ambient)
{
    this->defuse = defuse;
    this->emission = emission;
    this->ambient = ambient;
}

MaterialType MaterialSimple::getType()
{
    return MaterialType::Simple;
}
