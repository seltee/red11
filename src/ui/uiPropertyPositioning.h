// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"

class UIPropertyPositioning
{
public:
    inline UIBlockPositioning getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        bIsSet = false;
        value = UIBlockPositioning::Inline;
    }
    inline void set(UIBlockPositioning value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    UIBlockPositioning value = UIBlockPositioning::Inline;
};