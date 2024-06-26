// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9data.h"

Directx9data::Directx9data()
{
    ZeroMemory(meshRenderData, sizeof(Directx9MeshRenderData *) * MAX_MESH_COUNT);
    ZeroMemory(textureRenderData, sizeof(Directx9TextureRenderData *) * MAX_TEXTURES_COUNT);
    ZeroMemory(materialRenderData, sizeof(Directx9MaterialRenderData *) * MAX_MATERIALS_COUNT);
    ZeroMemory(queueMeshes, sizeof(QueuedMeshRenderData) * MAX_QUEUE_MESH_COUNT);
    ZeroMemory(queueLights, sizeof(QueuedLightRenderData) * MAX_QUEUE_LIGHTS_COUNT);
}

Directx9data::~Directx9data()
{
    // TODO Destroy all data
    printf("TODO: Destroy all data in dx9data\n");
}

Directx9MeshRenderData *Directx9data::getMeshRenderData(Mesh *mesh)
{
    if (!mesh)
        return nullptr;

    int meshIndex = mesh->getIndex();
    Directx9MeshRenderData *meshData = meshRenderData[meshIndex];
    if (!meshData)
    {
        meshData = new Directx9MeshRenderData(d3ddev, mesh);
        meshRenderData[meshIndex] = meshData;
    }
    return meshData;
}

Directx9MaterialRenderData *Directx9data::getMaterialRenderData(Material *material)
{
    if (!material)
        return nullptr;

    int materialIndex = material->getIndex();
    Directx9MaterialRenderData *materialData = materialRenderData[materialIndex];
    if (!materialData)
    {
        materialData = new Directx9MaterialRenderData(d3ddev, material);
        materialRenderData[materialIndex] = materialData;
    }
    return materialData;
}

Directx9TextureRenderData *Directx9data::getTextureRenderData(Texture *texture)
{
    if (!texture)
        return nullptr;

    int tIndex = texture->getIndex();

    auto textureDXData = textureRenderData[tIndex];
    if (!textureDXData)
    {
        textureDXData = new Directx9TextureRenderData(d3ddev, texture);
        if (!textureDXData->isReadyState)
        {
            delete textureDXData;
            textureDXData = nullptr;
        }
        textureRenderData[tIndex] = textureDXData;
    }

    return textureDXData;
}

void Directx9data::recalcDistanceInQueue(Vector3 &cameraPosition)
{
    for (int i = 0; i < queueCurrentMesh; i++)
    {
        if (queueMeshes[i].material->isAlphaPhase())
            queueMeshes[i].distance = glm::distance2(cameraPosition, queueMeshes[i].centroid);
    }
}