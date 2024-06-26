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

    inline unsigned char getRedAsUChar() { return r * 255; }
    inline unsigned char getGreenAsUChar() { return g * 255; }
    inline unsigned char getBlueAsUChar() { return b * 255; }
    inline unsigned char getAlphaAsUChar() { return a * 255; }
    inline unsigned int getAsUInt()
    {
        return (getAlphaAsUChar() << 24) | (getRedAsUChar() << 16) | (getGreenAsUChar() << 8) | (getBlueAsUChar());
    }
    inline float *getAsFloatArray() { return &r; }

    EXPORT Color &operator=(const Color &other);
};
