// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "material.h"
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/texture.h"

class MaterialSimple : public Material
{
public:
    EXPORT MaterialSimple();
    EXPORT MaterialSimple(Color defuseColor, Color emissionColor = Color(0.0f, 0.0f, 0.0f));
    EXPORT MaterialSimple(Texture *defuseTexture, Texture *emissionTexture = nullptr);

    EXPORT MaterialType getType() override;

    inline Color &getDefuseColor() { return defuseColor; }
    inline Color &getEmissionColor() { return emissionColor; }

    inline Texture *getDefuseTexture() { return defuseTexture; }
    inline Texture *getEmissionTexture() { return emissionTexture; }

protected:
    Color defuseColor = Color(0.64f, 0.64f, 0.46f);
    Color emissionColor = Color(0.0f, 0.0f, 0.0f);

    Texture *defuseTexture = nullptr;
    Texture *emissionTexture = nullptr;
};