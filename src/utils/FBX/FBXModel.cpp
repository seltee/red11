// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXModel.h"
#include "FBXAnimationCurveNode.h"

FBXModel::FBXModel(FBXNode *node)
{
    id = *static_cast<unsigned long *>(node->bindedData.at(0).data);
    name = std::string(reinterpret_cast<const char *>(node->bindedData.at(1).data));
    parentId = 0;
    position = Vector3(0.0f, 0.0f, 0.0f);
    rotation = Vector3(0.0f, 0.0f, 0.0f);
    scale = Vector3(1.0f, 1.0f, 1.0f);
    geometry = nullptr;

    FBXNode *properties = node->findNode("Properties70");
    if (properties)
    {
        for (auto &property : properties->children)
        {
            // n - none, p - position, r - rotation, s - scale
            int entryIndex = 0;
            char transformationType = 'n';
            Vector3 out;
            for (auto &p : property->bindedData)
            {
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Translation") == 0)
                    transformationType = 'p';
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Rotation") == 0)
                    transformationType = 'r';
                if (p.type == 'S' && strcmp(reinterpret_cast<char *>(p.data), "Lcl Scaling") == 0)
                    transformationType = 's';
                if (p.type == 'D' && entryIndex < 3)
                {
                    out[entryIndex] = static_cast<float>(*reinterpret_cast<double *>(p.data));
                    entryIndex++;
                }
            }
            if (transformationType == 'p')
                position = out;

            if (transformationType == 's')
                scale = out;

            if (transformationType == 'r')
                rotation = out * CONST_PI / 180.0f;
        }
    }
}

FBXModel::~FBXModel()
{
}

void FBXModel::addAttribute(FBXAttribute *attribute)
{
    if (attribute->isLimb())
        bIsLimb = true;
}