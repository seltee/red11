// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9data.h"
#include "utils/sphere.h"

Directx9data::Directx9data()
{
    ZeroMemory(meshRenderData, sizeof(Directx9MeshRenderData *) * MAX_ELEMENT_INDEX);
    ZeroMemory(textureRenderData, sizeof(Directx9TextureRenderData *) * MAX_ELEMENT_INDEX);
    ZeroMemory(materialRenderData, sizeof(Directx9MaterialRenderData *) * MAX_ELEMENT_INDEX);
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

void Directx9data::destroyMeshRenderDataByIndex(unsigned int index)
{
    if (meshRenderData[index])
    {
        delete meshRenderData[index];
        meshRenderData[index] = nullptr;
    }
}

void Directx9data::destroyMaterialRenderDataByIndex(unsigned int index)
{
    if (materialRenderData[index])
    {
        delete materialRenderData[index];
        materialRenderData[index] = nullptr;
    }
}

void Directx9data::destroyTextureRenderDataByIndex(unsigned int index)
{
    if (textureRenderData[index])
    {
        delete textureRenderData[index];
        textureRenderData[index] = nullptr;
    }
}

void Directx9data::recalcDistanceInQueue(Vector3 &cameraPosition)
{
    for (int i = 0; i < queueCurrentMesh; i++)
    {
        if (queueMeshes[i].material->isAlphaPhase())
            queueMeshes[i].distance = glm::distance2(cameraPosition, queueMeshes[i].centroid);
    }
}

void Directx9data::remakeActiveQueueForCamera(Camera *camera)
{
    queueActiveCurrentMesh = 0;
    queueActiveCurrentLight = 0;
    for (int i = 0; i < queueCurrentMesh; i++)
    {
        if (isMeshVisibleToCamera(&queueMeshes[i], camera))
        {
            queueActiveMeshes[queueActiveCurrentMesh] = &queueMeshes[i];
            queueActiveCurrentMesh++;
        }
    }
    for (int i = 0; i < queueCurrentLight; i++)
    {
        if (queueLights[i].light && isLightVisibleToCamera(&queueLights[i], camera))
        {
            queueActiveLights[queueActiveCurrentLight] = &queueLights[i];
            queueActiveCurrentLight++;
        }
    }
}

bool Directx9data::isMeshVisibleToCamera(QueuedMeshRenderData *mesh, Camera *camera)
{
    // todo proper bones check
    if (mesh->bones.size() > 0)
    {
        Sphere sphere;
        for (auto &bone : mesh->bones)
        {
            Matrix4 mv = *camera->getViewMatrix() * *bone.model;
            sphere.setup(Vector3(0), bone.deform->getCullingRadius());
            if (sphere.isSphereInFrustum(&mv, camera->getCullingPlanes()))
                return true;
        }
    }
    else
    {
        Matrix4 mv = *camera->getViewMatrix() * *mesh->model;
        if (mesh->mesh->getBoundVolumeSphere().isSphereInFrustum(&mv, camera->getCullingPlanes()))
            return true;
    }
    return false;
}

bool Directx9data::isLightVisibleToCamera(QueuedLightRenderData *light, Camera *camera)
{
    if (light->light->getType() == LightType::Directional)
        return true;

    if (light->light->getType() == LightType::Omni || light->light->getType() == LightType::Spot)
    {
        Vector3 position = light->light->getPosition();
        Sphere sphere;
        sphere.setup(position, light->light->getAffectDistance());
        if (sphere.isSphereInFrustum(camera->getViewMatrix(), camera->getCullingPlanes()))
            return true;
        return false;
    }
    return false;
}