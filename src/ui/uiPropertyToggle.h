// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyToggle
{
public:
    inline bool getValue() const { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
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
