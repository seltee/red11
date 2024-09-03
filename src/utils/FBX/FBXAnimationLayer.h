// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/FBX/FBXNode.h"
#include "utils/FBX/FBXAnimationCurveNode.h"
#include <vector>
#include <string>

class FBXAnimationLayer
{
public:
    FBXAnimationLayer(FBXNode *node);
    ~FBXAnimationLayer();

    void linkCurveNode(FBXAnimationCurveNode *node);

    unsigned long long id = 0;
    std::string name;

    std::vector<FBXAnimationCurveNode *> curveNodes;
};