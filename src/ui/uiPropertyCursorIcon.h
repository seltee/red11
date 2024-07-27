// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"
#include "window/window.h"

class UIPropertyCursorIcon
{
public:
    inline MouseCursorIcon getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        value = MouseCursorIcon::Default;
        bIsSet = false;
    }
    inline void set(MouseCursorIcon value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    MouseCursorIcon value = MouseCursorIcon::Default;
};
