// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"
#include "utils/primitives.h"

class UIPropertyColor
{
public:
    inline Color getValue() const { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
    inline void unSet()
    {
        value = Color(0, 0, 0, 1.0f);
        bIsSet = false;
    }
    inline void set(const Color &value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    Color value = Color(0, 0, 0, 1.0f);
};
