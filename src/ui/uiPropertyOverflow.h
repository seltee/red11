// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyOverflow
{
public:
    inline UIOverflow getValue() const { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
    inline void unSet()
    {
        bIsSet = false;
        value = UIOverflow::Visible;
    }
    inline void set(UIOverflow value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    UIOverflow value = UIOverflow::Visible;
};
