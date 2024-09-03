// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "FBXNode.h"
#include "utils/utils.h"
#include "utils/primitives.h"
#include <string>
#include <vector>

enum class FBXDeformType
{
    None,
    Skin,
    Cluster
};

class FBXDeform
{
public:
    FBXDeform(FBXNode *node);
    ~FBXDeform();

    bool hasDeformer(FBXDeform *deform);

    void print();

    static FBXDeformType getTypeByName(std::string type);

    inline std::vector<FBXDeform *> *getChildren() { return &children; }

    inline FBXDeformType getType() { return type; }
    inline std::string &getName() { return name; }

    inline int *getIndexies() { return indexes; }
    inline int getIndexiesAmount() { return indexesAmount; }

    inline float *getWeights() { return weights; }
    inline int getWeightsAmount() { return weightsAmount; }

    inline Matrix4 &getInvBindMatrix() { return mInvTransform; }

    void addChild(FBXDeform *child);
    void setParent(FBXDeform *parent);

    unsigned long long id;

protected:
    FBXDeformType type;
    std::string name;

    std::vector<FBXDeform *> children;
    FBXDeform *parent = nullptr;

    Matrix4 mInvTransform;

    int *indexes = nullptr;
    int indexesAmount = 0;

    float *weights = nullptr;
    int weightsAmount = 0;
};