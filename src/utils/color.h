// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

class Color
{
public:
    EXPORT Color();
    EXPORT Color(float r, float g, float b);
    EXPORT Color(float r, float g, float b, float a);

    float r, g, b, a;

    inline unsigned char getRedAsUChar() const { return r * 255; }
    inline unsigned char getGreenAsUChar() const { return g * 255; }
    inline unsigned char getBlueAsUChar() const { return b * 255; }
    inline unsigned char getAlphaAsUChar() const { return a * 255; }
    inline unsigned int getAsUInt()
    {
        return (getAlphaAsUChar() << 24) | (getRedAsUChar() << 16) | (getGreenAsUChar() << 8) | (getBlueAsUChar());
    }
    inline const float *getAsFloatArray() const { return &r; }

    EXPORT Color &operator=(const Color &other);
};
