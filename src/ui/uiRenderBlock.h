// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include "utils/primitives.h"

class UINode;

class UIRenderBlock
{
public:
    float x, y;
    float textShiftX, textShiftY;
    float imageShiftX, imageShiftY;
    float hoverShiftX, hoverShiftY;
    float hoverWidth;
    float hoverHeight;
    int index;
    UINode *source;
    std::vector<UIRenderBlock *> children;
    OverflowWindow overflowWindow;
};