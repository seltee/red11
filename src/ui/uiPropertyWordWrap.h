// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyWordWrap
{
public:
    inline UIWordWrap getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        bIsSet = false;
        value = UIWordWrap::Disabled;
    }
    inline void set(UIWordWrap value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    UIWordWrap value = UIWordWrap::Disabled;
};
