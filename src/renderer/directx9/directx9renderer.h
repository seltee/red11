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
#include "data/boneTransform.h"
#include "shaders/UVSimpleFragmentShader.pso.h"
#include "shaders/UVSimpleVertexShader.vso.h"
#include "shaders/UVSimpleMaskFragmentShader.pso.h"
#include "shaders/UVSimpleMaskVertexShader.vso.h"
#include "shaders/UVFragmentShader.pso.h"
#include "shaders/UVVertexShader.vso.h"
#include "shaders/UVNormalFragmentShader.pso.h"
#include "shaders/UVNormalVertexShader.vso.h"
#include "shaders/UVSkinnedFragmentShader.pso.h"
#include "shaders/UVSkinnedVertexShader.vso.h"
#include "shaders/UVSkinnedNormalVertexShader.vso.h"
#include "shaders/UVSkinnedNormalFragmentShader.pso.h"
#include "shaders/UVShadowVertexShader.vso.h"
#include "shaders/UVShadowFragmentShader.pso.h"
#include "shaders/UVShadowMaskVertexShader.vso.h"
#include "shaders/UVShadowMaskFragmentShader.pso.h"
#include "shaders/UVShadowSkinnedVertexShader.vso.h"
#include "directx9meshRenderData.h"
#include "directx9materialRenderData.h"
#include "directx9textureRenderData.h"
#include "directx9utils.h"
#include "directx9data.h"

#pragma comment(lib, "d3d9.lib")

class DirectX9Renderer : public Renderer
{
public:
    DirectX9Renderer(Window *window);

    RendererType getType();

    void clearBuffer(Color color);
    void queueMesh(Mesh *mesh, Material *material, Matrix4 *model);
    void queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones);
    void queueLine(Vector3 vFrom, Vector3 vTo, Color color);
    void queueLight(Light *light);
    void renderQueue(Camera *camera);
    void clearQueue();
    void renderMesh(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model);
    void renderMeshSkinned(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model, std::vector<BoneTransform> *bones);
    void renderLine(Camera *camera, Vector3 vFrom, Vector3 vTo);
    void setAmbientLight(Color &ambientColor);
    void present();

protected:
    void initD3D(HWND hWnd, bool bIsFullscreen, int width, int height); // sets up and initializes Direct3D
    void renderQueueDepthBuffer(Vector3 &cameraPosition, Camera *camera);
    void renderQueueLightDepthBuffer(Vector3 &cameraPosition, Camera *camera);
    void renderQueueDepthEqual(Vector3 &cameraPosition, Camera *camera);
    void cleanD3D(void);

    void setupLights(Vector3 objectPosition, float objectRadius);

    inline void setupMaterialColorRender(Material *material)
    {
        Directx9MaterialRenderData *materialRenderData = data.getMaterialRenderData(material);
        if (materialRenderData)
            materialRenderData->setupForRender(&data);
    }

    inline void setupMaterialDepthRender(Material *material)
    {
        Directx9MaterialRenderData *materialRenderData = data.getMaterialRenderData(material);
        if (materialRenderData)
            materialRenderData->setupForDepth(&data);
    }

    void renderMeshColorData(Camera *camera, Vector3 &cameraPosition, QueuedMeshRenderData *mesh);
    void renderMeshDepthData(Camera *camera, Vector3 &cameraPosition, QueuedMeshRenderData *mesh);
    void renderMeshShadowDepthData(Camera *camera, Vector3 &cameraPosition, QueuedMeshRenderData *mesh);

    void renderShadowBuffers(Vector3 &cameraPosition);
    void renderShadowBuffersDirectional(Vector3 &cameraPosition, Light *light);

    Color ambientColor = Color(1.0f, 1.0f, 1.0f);

    LPDIRECT3D9 d3d = nullptr;          // the pointer to our Direct3D interface
    LPDIRECT3DDEVICE9 d3ddev = nullptr; // the pointer to the device class

    Mesh *cubeMesh = nullptr;
    Material *lineMaterial = nullptr;

    // To render depth into depth buffer without color
    IDirect3DVertexShader9 *pUVSimpleVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVSimpleFragmentShader = nullptr;

    // To render depth into depth buffer without color but with mask of discarded alpha
    IDirect3DVertexShader9 *pUVSimpleMaskVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVSimpleMaskFragmentShader = nullptr;

    // Default fast shader with not normal maps
    IDirect3DVertexShader9 *pUVVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVFragmentShader = nullptr;

    // Default fast shader with normal maps
    IDirect3DVertexShader9 *pUVNormalVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVNormalFragmentShader = nullptr;

    // Default fast shader with bones but no normal maps
    IDirect3DVertexShader9 *pUVSkinnedVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVSkinnedFragmentShader = nullptr;

    // Default fast shader with bones but no normal maps
    IDirect3DVertexShader9 *pUVSkinnedNormalVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVSkinnedNormalFragmentShader = nullptr;

    // Shadow render of depth
    IDirect3DVertexShader9 *pUVShadowVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVShadowFragmentShader = nullptr;

    // Shadow render of depth
    IDirect3DVertexShader9 *pUVShadowMaskVertexShader = nullptr;
    IDirect3DPixelShader9 *pUVShadowMaskFragmentShader = nullptr;

    // Shadow skinned without mask, uses shadow fragment shader
    IDirect3DVertexShader9 *pUVShadowSkinnedVertexShader = nullptr;

    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUV = nullptr;
    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUVSkinned = nullptr;

    Material *defaultMaterial;

    Directx9data data;
};

#endif