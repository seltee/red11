// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyNumber
{
public:
    inline unsigned int getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        value = 0;
        bIsSet = false;
    }
    inline void set(unsigned int value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    unsigned int value = 0;
};
