// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <math.h>
#include <string>
#include <vector>

#define CONST_PI 3.1415926f
#define CONST_PI2 6.2831853f

#ifdef _WIN32
#define WINDOWS_ONLY
#endif

#ifdef __APPLE__
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT __declspec(dllexport)
#endif

#ifndef _strdup
#define _strdup strdup
#endif

enum class ColorMode
{
    Lit = 0,
    Alpha = 1,
    Addition = 2,
    Substraction = 3,
};

EXPORT float randf();
EXPORT float randf(float from, float to);

EXPORT void calcTangets(const float *v1, const float *v2, const float *v3, float *out);

EXPORT inline float warpAngle(float angle) { return -CONST_PI + fmodf(CONST_PI2 + fmodf(angle + CONST_PI, CONST_PI2), CONST_PI2); }