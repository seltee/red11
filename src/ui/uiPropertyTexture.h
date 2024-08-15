// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"
#include "data/font.h"

class UIPropertyTexture
{
public:
    inline Texture *getValue() { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
    inline void unSet()
    {
        bIsSet = false;
        value = nullptr;
    }
    inline void set(Texture *value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    Texture *value = nullptr;
};
