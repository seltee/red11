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
    float tangent[3];
    float bitangent[3];
};

struct DX9VertexNormalUVSkinned
{
    float x, y, z;
    float normalX, normalY, normalZ;
    float u, v;
    float tangent[3];
    float bitangent[3];
    unsigned char boneIndices[4];
    float boneWeights[4];
};

struct DX9LightShaderStruct
{
    float type;
    float data0;
    float data1;
    float data2;

    float position[3];
    float data3;

    float normal[3];
    float data4;

    float color[3];
    float data5;
};

//
struct DX9Material
{
    float useAlbedoTexture;
    float useNormalTexture;
    float useMetallicTexture;
    float useRoughnessTexture;

    float useAOTexture;
    float useEmissionTexture;
    float roughnessValue;
    float metallicValue;
};

#endif