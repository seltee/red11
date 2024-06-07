// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/FBX/FBXNode.h"
#include "utils/FBX/FBXAnimationCurve.h"
#include "utils/math.h"
#include "utils/utils.h"
#include <vector>
#include <string>

// An animation curve node is the connection point between animation curves and FBX properties.
// A curve node can be connected to only one property of one FBX object.
// Properties such as KFbxNode::LclTranslation contain more than one value (X, Y, Z).

struct FBXModelWithIndex;

// type n - none, x - x, y - y, z - z
struct FBXCurveBinding
{
    FBXAnimationCurve *curve;
    char axis;
};

enum class FBXAnimationCurveNodeType
{
    Unknown,
    Position,
    Scale,
    Rotation
};

class FBXAnimationCurveNode
{
public:
    EXPORT FBXAnimationCurveNode(FBXNode *node);
    EXPORT void linkCurve(FBXAnimationCurve *curve, FBXNode *node);
    EXPORT void addAffectedModel(FBXModelWithIndex *model);
    EXPORT const char *getTypeName();

    EXPORT bool hasModelName(std::string modelName);

    EXPORT FBXAnimationCurve *getXCurve();
    EXPORT FBXAnimationCurve *getYCurve();
    EXPORT FBXAnimationCurve *getZCurve();

    FBXAnimationCurveNodeType type;
    unsigned long long id;

    Vector3 defaultValue;

    std::vector<FBXCurveBinding> curves;
    std::vector<FBXModelWithIndex *> affectedModels;
};
