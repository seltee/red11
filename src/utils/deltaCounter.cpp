// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "deltaCounter.h"
#include <chrono>

DeltaCounter::DeltaCounter()
{
    lastTime = std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
                   .count();
    lastFPSCheck = lastTime;
}

float DeltaCounter::getDeltaFrameCounter(bool clearCounter)
{
    unsigned long long newTime = std::chrono::duration_cast<std::chrono::microseconds>(
                                     std::chrono::high_resolution_clock::now().time_since_epoch())
                                     .count();
    float dif = static_cast<float>(newTime - lastTime) / 1000000.0f;
    if (clearCounter)
        lastTime = newTime;

    if (newTime - lastFPSCheck > 1000000)
    {
        fps = frames;
        frames = 0;
        lastFPSCheck = newTime;
    }

    frames++;
    return dif;
}
