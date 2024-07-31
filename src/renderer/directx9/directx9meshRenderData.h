// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "settings.h"
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <d3d9.h>
#include <vector>
#include "data/mesh.h"
#include "directx9utils.h"

class Directx9MeshRenderData
{
public:
    Directx9MeshRenderData(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh);

    LPDIRECT3DVERTEXBUFFER9 vBuffer; // vertexies
    int vAmount;
    LPDIRECT3DINDEXBUFFER9 iBuffer; // indicies
    int iAmount;
    D3DFORMAT iFormat;

protected:
    void initPositionColor(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh);
    void initPositionUV(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh);
    void initPositionSkinned(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh);

    inline int getSmallestWeightIndex(float *boneWeights)
    {
        int smallest = 0;
        float weight = 1.0f;
        for (int i = 0; i < 4; i++)
        {
            if (boneWeights[i] < weight)
            {
                weight = boneWeights[i];
                smallest = i;
            }
        }
        return smallest;
    }
};

#endif