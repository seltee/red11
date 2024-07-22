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

EXPORT inline float warpAngle(float angle) { return -CONST_PI + fmodf(CONST_PI2 + fmodf(angle + CONST_PI, CONST_PI2), CONST_PI2); }