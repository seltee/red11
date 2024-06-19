// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include <string>
#include "FBXGeometry.h"
#include "FBXAttribute.h"
#include "data/mesh.h"
#include "utils/utils.h"
#include "data/meshObject.h"
#include "FBXNode.h"

class FBXAnimationCurveNode;

class FBXModel
{
public:
    EXPORT FBXModel(FBXNode *node);
    EXPORT ~FBXModel();

    EXPORT void addAttribute(FBXAttribute *attribute);

    inline void setParentId(unsigned long long parentId) { this->parentId = parentId; }
    inline unsigned long long getParentId() { return parentId; }

    inline std::string getName() { return name; }

    inline bool isLimb() { return bIsLimb; }

    unsigned long long id;
    unsigned long long parentId = 0;
    unsigned long long parentDeformId = 0;

    Vector3 position, rotation, scale;

    std::vector<FBXAnimationCurveNode *> curveNodes;

    FBXGeometry *geometry = nullptr;
    MeshObject *meshObject = nullptr;

protected:
    std::string name;

    bool bIsLimb = false;
};