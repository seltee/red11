// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"
#include <string>

class UIPropertyText
{
public:
    inline std::string &getValue() { return value; }
    inline bool isSet() { return bIsSet; }
    inline bool isNotSet() { return !bIsSet; }
    inline void unSet()
    {
        value = "";
        bIsSet = false;
    }
    inline void set(std::string value)
    {
        this->value = value;
        bIsSet = true;
    }

protected:
    bool bIsSet = false;
    std::string value = "";
};
