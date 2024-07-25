// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyDimension
{
public:
    inline float getValue() { return value; }
    inline bool isUsingPercentage() { return bIsUsingPercentage; }
    inline bool isUsingNumber() { return !bIsUsingPercentage; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        bIsUsingPercentage = false;
        value = 0.0f;
        bIsSet = false;
    }
    inline void setAsNumber(float value)
    {
        this->value = value;
        bIsUsingPercentage = false;
        bIsSet = true;
    }
    inline void setAsPercentage(float percentage)
    {
        this->value = percentage;
        bIsUsingPercentage = true;
        bIsSet = true;
    }

protected:
    bool bIsUsingPercentage = false;
    bool bIsSet = false;
    float value = 0.0f;
};
