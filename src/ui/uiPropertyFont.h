// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"
#include "data/font.h"

class UIPropertyFont
{
public:
    inline Font *getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        bIsSet = false;
        value = nullptr;
    }
    inline void set(Font *value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    Font *value = nullptr;
};
