// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <vector>
#include "data/mesh.h"
#include "renderer/renderer.h"

struct QueuedLightRenderData
{
    Light *light;
    bool enabled;
};

struct QueuedMeshRenderData
{
    Mesh *mesh;
    std::vector<BoneTransform> *bones;
    Material *material;
    Matrix4 *model;
};

struct QueuedLineRenderData
{
    Vector3 vFrom;
    Vector3 vTo;
    Color color;
};

struct DX9VertexNormalColor
{
    float x, y, z;
    float normalX, normalY, normalZ;
    unsigned int color;
};

struct DX9VertexNormalUV
{
    float x, y, z;
    float normalX, normalY, normalZ;
    float u, v;
};

struct DX9VertexNormalUVSkinned
{
    float x, y, z;
    float normalX, normalY, normalZ;
    float u, v;
    unsigned char boneIndices[4];
    float boneWeights[4];
};

#endif