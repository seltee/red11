// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include "renderer/renderer.h"
#include "data/mesh.h"
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "data/texture.h"

#pragma comment(lib, "d3d9.lib")

#define MAX_QUEUE_LIGHTS_COUNT (8 * 1024)
#define MAX_QUEUE_MESH_COUNT (256 * 1024)
#define MAX_TEXTURES_COUNT (32 * 1024)
#define MAX_LIGHTS_PER_MESH_COUNT 8

struct MeshRenderData
{
    LPDIRECT3DVERTEXBUFFER9 vBuffer; // vertexies
    int vAmount;
    LPDIRECT3DINDEXBUFFER9 iBuffer; // indicies
    int iAmount;
    D3DFORMAT iFormat;
};

struct MaterialRenderData
{
    D3DMATERIAL9 material;
};

struct TextureRenderData
{
    IDirect3DTexture9 *texture;
    int updIndex;
};

struct QueuedLightRenderData
{
    Light *light;
    bool enabled;
};

struct QueuedMeshRenderData
{
    Mesh *mesh;
    Material *material;
    Matrix4 *model;
};

class DirectX9Renderer : public Renderer
{
public:
    DirectX9Renderer(Window *window);

    RendererType getType();

    void startRendering();
    void clearBuffer(Color color);
    void queueMesh(Mesh *mesh, Material *material, Matrix4 *model);
    void queueLight(Light *light);
    void renderQueue(Camera *camera);
    void clearQueue();
    void renderMesh(Mesh *mesh, Material *material, Matrix4 *model);
    void setAmbientLight(Color &color);
    void setLightningState(bool value);
    void setZTestState(bool value);
    void endRendering();
    void present();

protected:
    void initD3D(HWND hWnd, bool bIsFullscreen, int width, int height); // sets up and initializes Direct3D
    void renderFrame(void);                                             // renders a single frame
    void cleanD3D(void);

    MeshRenderData *createBuffer(Mesh *mesh);
    MaterialRenderData *createSimpleMaterial(MaterialSimple *material);
    TextureRenderData *createTexture(Texture *texture);

    LPDIRECT3D9 d3d = nullptr;          // the pointer to our Direct3D interface
    LPDIRECT3DDEVICE9 d3ddev = nullptr; // the pointer to the device class

    MeshRenderData *meshRenderData[MAX_MESH_COUNT];
    MaterialRenderData *materialRenderData[MAX_MATERIAL_COUNT];
    TextureRenderData *textureRenderData[MAX_TEXTURES_COUNT];

    int queueCurrentLight = 0;
    int queueCurrentMesh = 0;
    QueuedMeshRenderData queueMeshes[MAX_QUEUE_MESH_COUNT];
    QueuedLightRenderData queueLights[MAX_QUEUE_LIGHTS_COUNT];

    bool useNearestLightsOnly = false;
};

#endif