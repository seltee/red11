// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyWordWrap
{
public:
    inline UIWordWrap getValue() const { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
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
