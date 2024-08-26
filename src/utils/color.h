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

    inline void normalize()
    {
        this->r = fmaxf(fminf(this->r, 1.0f), 0.0f);
        this->g = fmaxf(fminf(this->g, 1.0f), 0.0f);
        this->b = fmaxf(fminf(this->b, 1.0f), 0.0f);
        this->a = fmaxf(fminf(this->a, 1.0f), 0.0f);
    }

    inline Color &operator=(const Color &other)
    {
        if (this != &other) // not a self-assignment
        {
            this->r = other.r;
            this->g = other.g;
            this->b = other.b;
            this->a = other.a;
        }
        return *this;
    }

    inline Color &operator-(const Color &other)
    {
        if (this != &other) // not a self-assignment
        {
            this->r = this->r - other.r;
            this->g = this->g - other.g;
            this->b = this->b - other.b;
            this->a = this->a - other.a;
        }
        return *this;
    }

    inline Color &operator+(const Color &other)
    {
        if (this != &other) // not a self-assignment
        {
            this->r = this->r + other.r;
            this->g = this->g + other.g;
            this->b = this->b + other.b;
            this->a = this->a + other.a;
        }
        return *this;
    }

    inline Color &operator*(float value)
    {
        this->r = this->r * value;
        this->g = this->g * value;
        this->b = this->b * value;
        this->a = this->a * value;
        return *this;
    }

    inline Color &operator/(const Color &other)
    {
        if (this != &other) // not a self-assignment
        {
            this->r = this->r / other.r;
            this->g = this->g / other.g;
            this->b = this->b / other.b;
            this->a = this->a / other.a;
        }
        return *this;
    }
};
