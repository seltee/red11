// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "settings.h"
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
#include "shaders/SpriteVertexShader.vso.h"
#include "shaders/SpriteFragmentShader.pso.h"
#include "directx9meshRenderData.h"
#include "directx9materialRenderData.h"
#include "directx9textureRenderData.h"
#include "directx9utils.h"
#include "directx9data.h"
#include "directx9shader.h"

#pragma comment(lib, "d3d9.lib")

class DirectX9Renderer : public Renderer
{
public:
    EXPORT DirectX9Renderer(Window *window);

    EXPORT RendererType getType() override final;

    EXPORT void prepareToRender() override final;
    EXPORT void clearBuffer(Color color) override final;
    EXPORT void queueMesh(Mesh *mesh, Material *material, Matrix4 *model) override final;
    EXPORT void queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones) override final;
    EXPORT void queueLine(Vector3 vFrom, Vector3 vTo, Color color) override final;
    EXPORT void queueLight(Light *light) override final;
    EXPORT void renderQueue(Camera *camera) override final;
    EXPORT void clearQueue() override final;
    EXPORT void renderMesh(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model) override final;
    EXPORT void renderMeshSkinned(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model, std::vector<BoneTransform> *bones) override final;
    EXPORT void renderLine(Camera *camera, Vector3 vFrom, Vector3 vTo);

    EXPORT void setupSpriteRendering(Matrix4 &mView, Matrix4 &mProjection) override final;
    EXPORT void renderSpriteRect(Matrix4 *mModel, Color color) override final;
    EXPORT void renderSpriteMask(Matrix4 *mModel, Texture *texture, Color color) override final;
    EXPORT void renderSpriteImage(Matrix4 *mModel, Texture *texture) override final;

    EXPORT void setAmbientLight(Color &ambientColor) override final;

    EXPORT void present() override final;

    EXPORT void removeTextureByIndex(unsigned int index) override;
    EXPORT void removeMaterialByIndex(unsigned int index) override;
    EXPORT void removeMeshByIndex(unsigned int index) override;

    EXPORT DirectX9Shader *createDirectX9Shader(std::string name, const DWORD *vsoCode, const DWORD *psoCode);

    inline float *getShaderEngineDataPtr() { return engineData; }

protected:
    void initD3D(HWND hWnd, bool bIsFullscreen, int width, int height); // sets up and initializes Direct3D
    void resizeD3D(int width, int height);
    void renderQueueDepthBuffer(Vector3 &cameraPosition, Camera *camera);
    void renderQueueLightDepthBuffer(Vector3 &cameraPosition, Camera *camera);
    void renderQueueDepthEqual(Vector3 &cameraPosition, Camera *camera);
    void cleanD3D(void);

    // Bonned meshed limited to 4 shadows per mesh
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

    void renderShadowBuffers(Vector3 &cameraPosition, Vector3 &cameraFrowardVector);
    void renderShadowBuffersDirectional(Vector3 &cameraPosition, Vector3 &cameraFrowardVector, Light *light);
    void renderShadowBuffersSpot(Light *light);

    Color ambientColor = Color(1.0f, 1.0f, 1.0f);

    LPDIRECT3D9 d3d = nullptr;          // the pointer to our Direct3D interface
    LPDIRECT3DDEVICE9 d3ddev = nullptr; // the pointer to the device class

    Mesh *cubeMesh = nullptr;
    Mesh *spriteMesh = nullptr;
    Material *lineMaterial = nullptr;

    // Propageted to shader under c19, common to all. May be used for time, etc
    float engineData[4] = {0, 0, 0, 0};

    // Common textures
    Texture *white = nullptr;
    Texture *black = nullptr;

    // To render depth into depth buffer without color
    DirectX9Shader *UVSimpleShader = nullptr;

    // To render depth into depth buffer without color but with mask of discarded alpha
    DirectX9Shader *UVSimpleMaskShader = nullptr;

    // Default fast shader with not normal maps
    DirectX9Shader *UVShader = nullptr;

    // Default fast shader with normal maps
    DirectX9Shader *UVNormalShader = nullptr;

    // Default fast shader with bones but no normal maps
    DirectX9Shader *UVSkinnedShader = nullptr;

    // Default fast shader with bones but no normal maps
    DirectX9Shader *UVSkinnedNormalShader = nullptr;

    // Shadow render of depth
    DirectX9Shader *UVShadowShader = nullptr;

    // Shadow render of depth
    DirectX9Shader *UVShadowMaskShader = nullptr;

    // Shadow skinned without mask, uses shadow fragment shader
    DirectX9Shader *UVShadowSkinnedShader = nullptr;

    // Sprite shader
    DirectX9Shader *SpriteShader = nullptr;

    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUV = nullptr;
    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUVSkinned = nullptr;

    Material *defaultMaterial;

    Directx9data data;

    Matrix4 mSpriteViewProjection;
};

#endif