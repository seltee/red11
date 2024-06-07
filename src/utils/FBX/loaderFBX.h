// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "FBXNode.h"
#include "FBXAnimationStack.h"
#include "FBXAnimationCurveNode.h"
#include "FBXAnimationCurve.h"
#include "FBXAnimationLayer.h"
#include "data/animation/animation.h"
#include "data/mesh.h"
#include "data/meshObject.h"
#include "utils/meshDescriptor.h"
#include <vector>

const unsigned long long FBXTimeToMs = 46186158;

struct FBXMeshWithIndex
{
    Mesh *mesh;
    unsigned long long index;
};

struct FBXModelWithIndex
{
    unsigned long long index;
    unsigned long long parentIndex;
    Vector3 position, rotation, scale;
    std::string name;
    std::vector<FBXAnimationCurveNode *> curveNodes;
    MeshObject *meshObject;
};

struct FBXAnimationBinding
{
    AnimationKeyTranform keyTransform;
    std::string modelName;
};

class LoaderFBX
{
protected:
    LoaderFBX();

public:
    EXPORT static bool loadFBXFile(std::string path, std::vector<MeshObject *> *meshObjectsList, std::vector<Animation *> *animationsList);

protected:
    static void printAnimationStructure(std::vector<FBXAnimationStack *> &animStacks);

    static FBXModelWithIndex *getModelByIndex(unsigned long long id, std::vector<FBXModelWithIndex> &models);
    static FBXMeshWithIndex *getMeshByIndex(unsigned long long id, std::vector<FBXMeshWithIndex> &meshes);
    static FBXAnimationStack *getStackByIndex(unsigned long long id, std::vector<FBXAnimationStack *> &animStacks);
    static FBXAnimationLayer *getLayerByIndex(unsigned long long id, std::vector<FBXAnimationLayer *> &animLayers);
    static FBXAnimationCurveNode *getCurveNodeByIndex(unsigned long long id, std::vector<FBXAnimationCurveNode *> &animCurveNodes);
    static FBXAnimationCurve *getCurveByIndex(unsigned long long id, std::vector<FBXAnimationCurve *> &animCurves);

    static std::vector<std::string> getAnimationNames(std::vector<FBXAnimationLayer *> &animLayers);

    static void gatherTimeStamps(FBXAnimationLayer *layer, std::vector<unsigned long long> *timestamps);
    static float getCurveLerped(FBXAnimationCurve *curve, float time);
};
