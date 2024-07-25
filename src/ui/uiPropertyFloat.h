// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyFloat
{
public:
    inline float getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        value = 0;
        bIsSet = false;
    }
    inline void set(float value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    float value = 0;
};
