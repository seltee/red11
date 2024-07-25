// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>

class UINode;

class UIRenderBlock
{
public:
    float x, y;
    int index;
    UINode *source;
    std::vector<UIRenderBlock *> children;
};