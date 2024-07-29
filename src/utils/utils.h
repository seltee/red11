// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "defines.h"
#include <math.h>
#include <string>
#include <vector>

EXPORT float randf();
EXPORT float randf(float from, float to);

EXPORT void calcTangets(const float *v1, const float *v2, const float *v3, float *out);

inline float warpAngle(float angle) { return -CONST_PI + fmodf(CONST_PI2 + fmodf(angle + CONST_PI, CONST_PI2), CONST_PI2); }

inline unsigned int nextPowerOfTwo(unsigned int n)
{
    if (n == 0)
        return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

EXPORT int getFileByteSize(std::string path);
EXPORT std::string convertWCharToString(const wchar_t *wcharStr);
