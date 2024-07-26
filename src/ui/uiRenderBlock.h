// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>

class UINode;

class UIRenderBlock
{
public:
    float x, y;
    float textShiftX, textShiftY;
    float imageShiftX, imageShiftY;
    int index;
    UINode *source;
    std::vector<UIRenderBlock *> children;
};