// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "FBXNode.h"
#include "FBXAnimationStack.h"
#include "FBXAnimationCurveNode.h"
#include "FBXAnimationCurve.h"
#include "FBXAnimationLayer.h"
#include "FBXDeform.h"
#include "FBXGeometry.h"
#include "FBXModel.h"
#include "FBXAttribute.h"
#include "data/animation/animation.h"
#include "data/mesh.h"
#include "data/meshObject.h"
#include <vector>

const unsigned long long FBXTimeToMs = 46186158;

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

    static FBXModel *getModelById(unsigned long long id, std::vector<FBXModel *> &models);
    static FBXAnimationStack *getStackById(unsigned long long id, std::vector<FBXAnimationStack *> &animStacks);
    static FBXAnimationLayer *getLayerById(unsigned long long id, std::vector<FBXAnimationLayer *> &animLayers);
    static FBXAnimationCurveNode *getCurveNodeById(unsigned long long id, std::vector<FBXAnimationCurveNode *> &animCurveNodes);
    static FBXAnimationCurve *getCurveById(unsigned long long id, std::vector<FBXAnimationCurve *> &animCurves);
    static FBXDeform *getDeformById(unsigned long long id, std::vector<FBXDeform *> &deforms);
    static FBXGeometry *getGeometryById(unsigned long long id, std::vector<FBXGeometry *> &geometries);
    static FBXAttribute *getAttributeById(unsigned long long id, std::vector<FBXAttribute *> &attributes);

    static FBXModel *getModelByDeform(FBXDeform *deform, std::vector<FBXModel *> &models);

    static std::vector<std::string> getAnimationNames(std::vector<FBXAnimationLayer *> &animLayers);

    static void gatherTimeStamps(FBXAnimationLayer *layer, std::vector<unsigned long long> *timestamps);
    static float getCurveLerped(FBXAnimationCurve *curve, float time);
};
