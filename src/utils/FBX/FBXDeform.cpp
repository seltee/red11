// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXDeform.h"
#include "utils/glm/gtx/matrix_decompose.hpp"

FBXDeform::FBXDeform(FBXNode *node)
{
    this->id = *static_cast<unsigned long long *>(node->bindedData.at(0).data);
    this->name = std::string(reinterpret_cast<const char *>(node->bindedData.at(1).data));
    this->type = FBXDeform::getTypeByName(std::string(reinterpret_cast<const char *>(node->bindedData.at(2).data)));

    FBXNode *indexes = node->findNode("Indexes");
    FBXNode *weights = node->findNode("Weights");
    FBXNode *transform = node->findNode("Transform");
    // FBXNode *transformLink = node->findNode("TransformLink");
    // FBXNode *transformAssociateModel = node->findNode("TransformAssociateModel");

    if (indexes)
    {
        this->indexes = reinterpret_cast<int *>(indexes->bindedData.at(0).data);
        this->indexesAmount = indexes->bindedData.at(0).numElements;
    }

    if (weights)
    {
        double *dWeights = reinterpret_cast<double *>(weights->bindedData.at(0).data);
        this->weightsAmount = weights->bindedData.at(0).numElements;
        this->weights = new float[this->weightsAmount];
        for (int i = 0; i < this->weightsAmount; i++)
            this->weights[i] = (float)dWeights[i];
    }

    if (transform)
    {
        for (int i = 0; i < 16; i++)
        {
            float v = reinterpret_cast<double *>(transform->bindedData.at(0).data)[i];
            mInvTransform[i / 4][i % 4] = v;
        }
    }
}

FBXDeform::~FBXDeform()
{
    if (this->weights)
        delete[] this->weights;
}

bool FBXDeform::hasDeformer(FBXDeform *deform)
{
    for (auto &it : children)
    {
        if (it == deform || it->hasDeformer(deform))
            return true;
    }
    return false;
}

void FBXDeform::print()
{
    printf("Deform %s (%llu):\n", name.c_str(), id);
    if (type == FBXDeformType::Skin)
        printf("Skin\n");
    if (type == FBXDeformType::Cluster)
        printf("Cluster\n");

    printf("Num indexes: %i\n", indexesAmount);
    printf("Num weights: %i\n", weightsAmount);

    printf("\n");
}

FBXDeformType FBXDeform::getTypeByName(std::string type)
{
    if (type == "Skin")
        return FBXDeformType::Skin;
    if (type == "Cluster")
        return FBXDeformType::Cluster;
    return FBXDeformType::None;
}

void FBXDeform::addChild(FBXDeform *child)
{
    this->children.push_back(child);
}

void FBXDeform::setParent(FBXDeform *parent)
{
    this->parent = parent;
}

/*
- Deformer
Deformer
 Deformer
 * 394217915(L)
 * Bone_left(S)
 * Cluster(S)
  Version
  * 0(I)
  UserData
  Indexes
  * int array (num 8)(i)
  Weights
  * Double array (num 8)(d)
  Transform
  * Double array (num 16)(d)
  TransformLink
  * Double array (num 16)(d)
  TransformAssociateModel
  * Double array (num 16)(d)

*/