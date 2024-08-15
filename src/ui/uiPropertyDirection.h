// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyDirection
{
public:
    inline UIContentDirection getValue() const { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
    inline void unSet()
    {
        value = UIContentDirection::Horizontal;
        bIsSet = false;
    }
    inline void set(UIContentDirection value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    UIContentDirection value = UIContentDirection::Horizontal;
};
