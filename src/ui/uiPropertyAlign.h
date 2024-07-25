// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyAlign
{
public:
    inline UIContentAlign getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        value = UIContentAlign::Start;
        bIsSet = false;
    }
    inline void set(UIContentAlign value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    UIContentAlign value = UIContentAlign::Start;
};
