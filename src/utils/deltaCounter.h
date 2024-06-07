// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

class DeltaCounter
{
public:
    EXPORT DeltaCounter();

    EXPORT float getDelta(bool clearCounter = true);

protected:
    unsigned long long lastTime;
};