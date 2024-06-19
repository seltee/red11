// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "deform.h"

Deform::Deform(std::string &name, DeformIndex *deformIndexData, int amount, Matrix4 &invBindMatrix)
{
    this->name = name;
    this->invBindMatrix = invBindMatrix;

    if (amount)
    {
        this->deformIndexData = new DeformIndex[amount];
        memcpy(this->deformIndexData, deformIndexData, sizeof(DeformIndex) * amount);
        this->deformIndexDataAmount = amount;
    }
}

Deform::Deform(std::string &name, int *indexies, float *weights, int amount, Matrix4 &invBindMatrix)
{
    this->name = name;
    this->invBindMatrix = invBindMatrix;

    if (amount)
    {
        this->deformIndexData = new DeformIndex[amount];
        for (int i = 0; i < amount; i++)
        {
            this->deformIndexData[i].index = indexies[i];
            this->deformIndexData[i].weight = weights[i];
        }
        this->deformIndexDataAmount = amount;
    }
}

Deform::~Deform()
{
    if (this->deformIndexData)
    {
        delete[] this->deformIndexData;
    }
}

float Deform::getWeightForIndex(int vIndex)
{
    for (int i = 0; i < deformIndexDataAmount; i++)
    {
        if (this->deformIndexData[i].index == vIndex)
            return this->deformIndexData[i].weight;
    }
    return 0.0f;
}