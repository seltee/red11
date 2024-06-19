// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <vector>
#include "renderer/renderer.h"
#include "shaders/fragmentColorShader.pso.h"
#include "shaders/vertexColorShader.vso.h"
#include "shaders/fragmentUVShader.pso.h"
#include "shaders/vertexUVShader.vso.h"
#include "shaders/fragmentUVSkinnedShader.pso.h"
#include "shaders/vertexUVSkinnedShader.vso.h"
#include "directx9meshRenderData.h"
#include "directx9textureRenderData.h"
#include "directx9utils.h"

#pragma comment(lib, "d3d9.lib")

#define MAX_QUEUE_LIGHTS_COUNT (8 * 1024)
#define MAX_QUEUE_MESH_COUNT (256 * 1024)
#define MAX_QUEUE_LINES (1 * 1024)
#define MAX_TEXTURES_COUNT (32 * 1024)
#define MAX_LIGHTS_PER_MESH_COUNT 8

#define LineFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

class DirectX9Renderer : public Renderer
{
public:
    DirectX9Renderer(Window *window);

    RendererType getType();

    void startRendering();
    void clearBuffer(Color color);
    void queueMesh(Mesh *mesh, Material *material, Matrix4 *model);
    void queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones);
    void queueLine(Vector3 vFrom, Vector3 vTo, Color color);
    void queueLight(Light *light);
    void renderQueue(Camera *camera);
    void clearQueue();
    void renderMesh(Camera *camera, Mesh *mesh, Material *material, Matrix4 *model);
    void renderMeshSkinned(Camera *camera, Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones);
    void renderLine(Camera *camera, Vector3 vFrom, Vector3 vTo, Color color);
    void setAmbientLight(Color &ambientColor);
    void setLightningState(bool value);
    void setDepthBufferState(bool state);
    void endRendering();
    void present();

protected:
    void initD3D(HWND hWnd, bool bIsFullscreen, int width, int height); // sets up and initializes Direct3D
    void renderFrame(void);                                             // renders a single frame
    void cleanD3D(void);

    Color ambientColor = Color(1.0f, 1.0f, 1.0f);

    Directx9MeshRenderData *getMeshRenderData(Mesh *mesh);
    Directx9TextureRenderData *getTextureRenderData(Texture *texture);

    LPDIRECT3D9 d3d = nullptr;          // the pointer to our Direct3D interface
    LPDIRECT3DDEVICE9 d3ddev = nullptr; // the pointer to the device class

    Directx9MeshRenderData *meshRenderData[MAX_MESH_COUNT];
    Directx9TextureRenderData *textureRenderData[MAX_TEXTURES_COUNT];

    int queueCurrentMesh = 0;
    QueuedMeshRenderData queueMeshes[MAX_QUEUE_MESH_COUNT];

    int queueCurrentLight = 0;
    QueuedLightRenderData queueLights[MAX_QUEUE_LIGHTS_COUNT];

    int queueCurrentLine = 0;
    QueuedLineRenderData queueLines[MAX_QUEUE_LINES];

    bool useNearestLightsOnly = false;

    Mesh *cubeMesh = nullptr;
    Material *lineMaterial = nullptr;

    IDirect3DVertexShader9 *pVertexColorShader = nullptr;
    IDirect3DPixelShader9 *pFragmentColorShader = nullptr;
    IDirect3DVertexShader9 *pVertexUVShader = nullptr;
    IDirect3DPixelShader9 *pFragmentUVShader = nullptr;
    IDirect3DVertexShader9 *pVertexUVSkinnedShader = nullptr;
    IDirect3DPixelShader9 *pFragmentUVSkinnedShader = nullptr;

    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalColor = nullptr;
    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUV = nullptr;
    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUVSkinned = nullptr;
};

#endif