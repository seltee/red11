// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9meshRenderData.h"

Directx9MeshRenderData::Directx9MeshRenderData(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh)
{
    iAmount = mesh->getPolygonsAmount();
    vAmount = mesh->getVerticiesAmount();
    auto polies = mesh->getPolygons();

    unsigned int indeciesAmount = iAmount * 3;
    bool useShortForIndicies = indeciesAmount < 0xffff;

    // if mesh data not intended to use UV
    if (mesh->getType() == VertexDataType::PositionColor)
        initPositionColor(d3ddev, mesh);

    // if mesh vertex data is just position/normal
    if (mesh->getType() == VertexDataType::PositionUV)
    {
        if (mesh->getDeforms()->size() > 0)
            initPositionSkinned(d3ddev, mesh);
        else
            initPositionUV(d3ddev, mesh);
    }

    // Indicies buffer
    iFormat = useShortForIndicies ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
    d3ddev->CreateIndexBuffer(useShortForIndicies ? indeciesAmount * sizeof(short) : indeciesAmount * sizeof(int),
                              0,
                              useShortForIndicies ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
                              D3DPOOL_MANAGED,
                              &iBuffer,
                              NULL);

    void *iBufferData;
    iBuffer->Lock(0, 0, (void **)&iBufferData, 0);

    for (int i = 0; i < iAmount; i++)
    {
        if (iFormat == D3DFMT_INDEX16)
        {
            ((unsigned short *)iBufferData)[i * 3] = polies[i].a;
            ((unsigned short *)iBufferData)[i * 3 + 1] = polies[i].b;
            ((unsigned short *)iBufferData)[i * 3 + 2] = polies[i].c;
        }
        else
        {
            ((unsigned int *)iBufferData)[i * 3] = polies[i].a;
            ((unsigned int *)iBufferData)[i * 3 + 1] = polies[i].b;
            ((unsigned int *)iBufferData)[i * 3 + 2] = polies[i].c;
        }
    }

    iBuffer->Unlock();
}

void Directx9MeshRenderData::initPositionColor(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh)
{
    auto verticies = mesh->getVerticies();
    d3ddev->CreateVertexBuffer(vAmount * sizeof(DX9VertexNormalColor), 0, 0, D3DPOOL_MANAGED, &vBuffer, NULL);

    DX9VertexNormalColor *vBufferData;
    vBuffer->Lock(0, 0, (void **)&vBufferData, 0); // locks v_buffer, the buffer we made earlier
    for (int i = 0; i < vAmount; i++)
    {
        VertexDataColored v = verticies->vertexPositionColor[i];
        vBufferData[i] = {v.position.x, v.position.y, v.position.z, v.position.x, v.position.y, v.position.z, Color(0.6f, 0.6f, 0.6f).getAsUInt()};
    }
    vBuffer->Unlock(); // unlock v_buffer
}

void Directx9MeshRenderData::initPositionUV(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh)
{
    auto verticies = mesh->getVerticies();
    d3ddev->CreateVertexBuffer(vAmount * sizeof(DX9VertexNormalUV), 0, 0, D3DPOOL_MANAGED, &vBuffer, NULL);
    DX9VertexNormalUV *vBufferData;
    vBuffer->Lock(0, 0, (void **)&vBufferData, 0); // locks v_buffer, the buffer we made earlier

    for (int i = 0; i < vAmount; i++)
    {
        VertexDataUV v = verticies->vertexPositionUV[i];

        vBufferData[i] = {
            v.position.x,
            v.position.y,
            v.position.z,
            v.normal.x,
            v.normal.y,
            v.normal.z,
            v.uv.x,
            v.uv.y,
            {v.tangent.x, v.tangent.y, v.tangent.z},
            {v.bitangent.x, v.bitangent.y, v.bitangent.z},
        };
    }

    vBuffer->Unlock(); // unlock v_buffer
}

void Directx9MeshRenderData::initPositionSkinned(LPDIRECT3DDEVICE9 d3ddev, Mesh *mesh)
{
    auto verticies = mesh->getVerticies();
    d3ddev->CreateVertexBuffer(vAmount * sizeof(DX9VertexNormalUVSkinned), 0, 0, D3DPOOL_MANAGED, &vBuffer, NULL);
    DX9VertexNormalUVSkinned *vBufferData;
    vBuffer->Lock(0, 0, (void **)&vBufferData, 0); // locks v_buffer, the buffer we made earlier
    std::vector<Deform *> *deforms = mesh->getDeforms();

    for (int i = 0; i < vAmount; i++)
    {
        VertexDataUV v = verticies->vertexPositionUV[i];

        float boneWeights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        unsigned char boneIndices[4] = {0, 0, 0, 0};

        for (auto &deform : *deforms)
        {
            float weight = deform->getWeightForIndex(v.index);
            if (weight > 0.0f)
            {
                int smallestWeightIndex = getSmallestWeightIndex(boneWeights);
                boneWeights[smallestWeightIndex] = weight;
                boneIndices[smallestWeightIndex] = deform->index;
            }
        }

        float totalWeight = 0.0f;
        for (int b = 0; b < 4; b++)
            totalWeight += boneWeights[b];
        for (int b = 0; b < 4; b++)
            boneWeights[b] /= totalWeight;

        vBufferData[i] = {
            v.position.x,
            v.position.y,
            v.position.z,
            v.normal.x,
            v.normal.y,
            v.normal.z,
            v.uv.x,
            v.uv.y,
            {v.tangent.x, v.tangent.y, v.tangent.z},
            {v.bitangent.x, v.bitangent.y, v.bitangent.z},
            {boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]},
            {boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]},
        };
    }
    vBuffer->Unlock(); // unlock v_buffer
}
