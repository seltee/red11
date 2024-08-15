// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "uiUtils.h"
#include <string>

class UIPropertyText
{
public:
    inline const std::string &getValue() const { return value; }
    inline bool isSet() const { return bIsSet; }
    inline bool isNotSet() const { return !bIsSet; }
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
