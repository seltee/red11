// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/boneTransform.h"
#include "directx9meshRenderData.h"
#include "directx9materialRenderData.h"
#include "directx9textureRenderData.h"
#include "directx9utils.h"
#include "utils/sphere.h"
#include <vector>

#ifdef WINDOWS_ONLY
#define MAX_QUEUE_LIGHTS_COUNT (1 * 1024)
#define MAX_QUEUE_MESH_COUNT (128 * 1024)
#define MAX_QUEUE_LINES (1 * 1024)
#define MAX_TEXTURES_COUNT (128 * 1024)
#define MAX_MATERIALS_COUNT (32 * 1024)
#define MAX_LIGHTS_PER_MESH_COUNT 16

class Directx9data
{
public:
    Directx9data();
    ~Directx9data();

    inline void addMesh(Mesh *mesh, Material *material, Matrix4 *model)
    {
        if (queueCurrentMesh < MAX_QUEUE_MESH_COUNT && mesh)
        {
            queueMeshes[queueCurrentMesh].mesh = mesh;
            queueMeshes[queueCurrentMesh].material = material;
            queueMeshes[queueCurrentMesh].model = model;
            queueMeshes[queueCurrentMesh].bones = nullptr;
            if (material->isAlphaPhase())
                queueMeshes[queueCurrentMesh].centroid = Vector3(mesh->getCentroid() * *model);
            queueCurrentMesh++;
        }
    }

    inline void addMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones)
    {
        if (queueCurrentMesh < MAX_QUEUE_MESH_COUNT && mesh)
        {
            queueMeshes[queueCurrentMesh].mesh = mesh;
            queueMeshes[queueCurrentMesh].material = material;
            queueMeshes[queueCurrentMesh].model = model;
            queueMeshes[queueCurrentMesh].bones = bones;
            if (material->isAlphaPhase())
                queueMeshes[queueCurrentMesh].centroid = Vector3(mesh->getCentroid() * *model);
            queueCurrentMesh++;
        }
    }

    inline void addLine(Vector3 vFrom, Vector3 vTo, Color color)
    {
        if (queueCurrentLine < MAX_QUEUE_LINES)
        {
            queueLines[queueCurrentLine].vFrom = vFrom;
            queueLines[queueCurrentLine].vTo = vTo;
            queueLines[queueCurrentLine].color = color;
            queueCurrentLine++;
        }
    }

    inline void addLight(Light *light)
    {
        if (queueCurrentLight < MAX_QUEUE_LIGHTS_COUNT)
        {
            queueLights[queueCurrentLight].light = light;
            queueCurrentLight++;
        }
    }

    inline void clearQueue()
    {
        queueCurrentLight = 0;
        queueCurrentMesh = 0;
        queueCurrentLine = 0;
    }

    Directx9MeshRenderData *getMeshRenderData(Mesh *mesh);
    Directx9MaterialRenderData *getMaterialRenderData(Material *material);
    Directx9TextureRenderData *getTextureRenderData(Texture *texture);

    void destroyMeshRenderDataByIndex(unsigned int index);
    void destroyMaterialRenderDataByIndex(unsigned int index);
    void destroyTextureRenderDataByIndex(unsigned int index);

    void recalcDistanceInQueue(Vector3 &cameraPosition);

    void remakeActiveQueueForCamera(Camera *camera);
    bool isMeshVisibleToCamera(QueuedMeshRenderData *mesh, Camera *camera);
    bool isLightVisibleToCamera(QueuedLightRenderData *light, Camera *camera);

    Directx9MeshRenderData *meshRenderData[MAX_MESH_COUNT];
    Directx9TextureRenderData *textureRenderData[MAX_TEXTURES_COUNT];
    Directx9MaterialRenderData *materialRenderData[MAX_MATERIALS_COUNT];

    int queueCurrentMesh = 0;
    QueuedMeshRenderData queueMeshes[MAX_QUEUE_MESH_COUNT];

    int queueActiveCurrentMesh = 0;
    QueuedMeshRenderData *queueActiveMeshes[MAX_QUEUE_MESH_COUNT];

    int queueCurrentLight = 0;
    QueuedLightRenderData queueLights[MAX_QUEUE_LIGHTS_COUNT];

    int queueActiveCurrentLight = 0;
    QueuedLightRenderData *queueActiveLights[MAX_QUEUE_LIGHTS_COUNT];

    int queueCurrentLine = 0;
    QueuedLineRenderData queueLines[MAX_QUEUE_LINES];

    LPDIRECT3DDEVICE9 d3ddev = nullptr;
};
#endif