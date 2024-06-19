// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include "data/mesh.h"
#include "utils/utils.h"
#include "FBXNode.h"

class FBXAttribute
{
public:
    EXPORT FBXAttribute(FBXNode *node);
    EXPORT ~FBXAttribute();

    EXPORT void print();

    unsigned long long id;

    inline bool isLimb() { return bIsLimb; }

protected:
    bool bIsLimb = false;
};