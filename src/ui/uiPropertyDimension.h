// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyDimension
{
public:
    inline float getValue() const { return value; }
    inline bool isUsingPercentage() const { return bIsUsingPercentage; }
    inline bool isUsingNumber() const { return !bIsUsingPercentage; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
    inline void unSet()
    {
        value = 0.0f;
        bIsUsingPercentage = false;
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
