// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyToggle
{
public:
    inline bool getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        value = false;
        bIsSet = false;
    }
    inline void set(bool value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    bool value = false;
};
