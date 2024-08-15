// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include <string>
#include "utils/utils.h"
#include "utils/primitives.h"

class Mesh;

struct DeformIndex
{
    int index;
    float weight;
};

class Deform
{
public:
    Deform(const std::string &name, DeformIndex *deformIndexData, int amount, const Matrix4 &invBindMatrix);
    Deform(const std::string &name, int *indexies, float *weights, int amount, const Matrix4 &invBindMatrix);
    ~Deform();

    void udpateCullingRadius(Mesh *mesh);

    inline const std::string &getName() const { return name; }
    inline bool isName(const std::string &name) const { return this->name == name; }

    inline const Matrix4 &getInvBindMatrix() const { return invBindMatrix; }

    inline bool hasIndex(int index) const
    {
        if (deformIndexDataAmount == 0)
            return false;

        for (int i = 0; i < deformIndexDataAmount; i++)
        {
            if (deformIndexData[i].index == index)
                return true;
        }
        return false;
    }

    float getWeightForIndex(int vIndex) const;

    inline int getIndexAmount() const { return deformIndexDataAmount; }
    inline int getVertIndexByIndex(int index) const { return deformIndexData[index].index; }

    inline float getCullingRadius() const { return cullingRadius; }

    unsigned int index = 0;

protected:
    std::string name;

    Matrix4 invBindMatrix;

    DeformIndex *deformIndexData = nullptr;
    int deformIndexDataAmount = 0;

    // Needed for culling
    float cullingRadius = 0;
};