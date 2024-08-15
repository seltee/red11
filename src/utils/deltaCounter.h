// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

class DeltaCounter
{
public:
    EXPORT DeltaCounter();

    EXPORT float getDeltaFrameCounter(bool clearCounter = true);
    inline int getFPS() const { return fps; }

protected:
    unsigned long long lastTime;
    unsigned long long lastFPSCheck;
    int fps = 0;
    int frames = 0;
};