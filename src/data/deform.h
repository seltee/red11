// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include <string>
#include "utils/utils.h"
#include "utils/primitives.h"

struct DeformIndex
{
    int index;
    float weight;
};

class Deform
{
public:
    Deform(std::string &name, DeformIndex *deformIndexData, int amount, Matrix4 &invBindMatrix);
    Deform(std::string &name, int *indexies, float *weights, int amount, Matrix4 &invBindMatrix);
    ~Deform();

    inline std::string &getName() { return name; }
    inline bool isName(std::string &name) { return this->name == name; }

    inline Matrix4 &getInvBindMatrix() { return invBindMatrix; }

    inline bool hasIndex(int index)
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

    float getWeightForIndex(int vIndex);

    unsigned int index = 0;

protected:
    std::string name;

    Matrix4 invBindMatrix;

    DeformIndex *deformIndexData = nullptr;
    int deformIndexDataAmount = 0;
};