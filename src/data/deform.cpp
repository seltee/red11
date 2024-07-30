// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "deform.h"
#include "data/mesh.h"

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

void Deform::udpateCullingRadius(Mesh *mesh)
{
    cullingRadius = 0;
    VertexData *verticies = mesh->getVerticies();
    int verticiesAmount = mesh->getVerticiesAmount();
    Matrix4 model = glm::inverse(invBindMatrix);
    Vector3 center = Vector3(model * Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    for (int i = 0; i < deformIndexDataAmount; i++)
    {
        int index = this->deformIndexData[i].index;
        int vIndex = -1;
        for (int i = 0; i < verticiesAmount; i++)
        {
            if (verticies->vertexPositionUV[i].index == index)
            {
                vIndex = i;
                break;
            }
        }
        if (vIndex != -1)
        {
            Vector3 pos = verticies->vertexPositionUV[vIndex].position;
            float dist = glm::length(center - pos);
            if (dist > cullingRadius)
                cullingRadius = dist;
        }
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