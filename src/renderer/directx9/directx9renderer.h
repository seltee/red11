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
#include "shaders/UVSkinnedVertexShader.vso.h"
#include "shaders/UVSkinnedNormalVertexShader.vso.h"
#include "shaders/UVShadowVertexShader.vso.h"
#include "shaders/UVShadowFragmentShader.pso.h"
#include "shaders/UVShadowMaskVertexShader.vso.h"
#include "shaders/UVShadowMaskFragmentShader.pso.h"
#include "shaders/UVShadowSkinnedVertexShader.vso.h"
#include "shaders/SpriteVertexShader.vso.h"
#include "shaders/SpriteFragmentShader.pso.h"
#include "shaders/skyHDRVertexShader.vso.h"
#include "shaders/skyHDRFragmentShader.pso.h"
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
    EXPORT DirectX9Renderer(Window *window, AntialiasingMethod antialiasingMethod, bool bVSync);

    EXPORT RendererType getType() override final;

    EXPORT void prepareToRender(Texture *ambientTexture = nullptr, Texture *radianceTexture = nullptr) override final;
    EXPORT void clearBuffer(const Color &color) override final;
    EXPORT void renderCubeMap(Camera *camera, Entity *entity, Texture *hdr) override final;
    EXPORT void queueMesh(Mesh *mesh, Material *material, const Matrix4 *model) override final;
    EXPORT void queueMeshSkinned(Mesh *mesh, Material *material, const Matrix4 *model, std::vector<BoneTransform> *bones) override final;
    EXPORT void queueLine(const Vector3 &vFrom, const Vector3 &vTo, const Color &color) override final;
    EXPORT void queueLight(Light *light) override final;
    EXPORT void renderQueue(Camera *camera) override final;
    EXPORT void clearQueue() override final;
    EXPORT void renderMesh(Camera *camera, Mesh *mesh, const Matrix4 *model) override final;
    EXPORT void renderMeshSkinned(Camera *camera, Mesh *mesh, std::vector<BoneTransform> *bones) override final;
    EXPORT void renderLine(Camera *camera, const Vector3 &vFrom, const Vector3 &vTo);

    EXPORT void setupSpriteRendering(const Matrix4 &mView, const Matrix4 &mProjection) override final;
    EXPORT void endSpriteRendering() override final;
    EXPORT void renderSpriteRect(const Matrix4 &mModel, const Color &color) override final;
    EXPORT void renderSpriteMask(const Matrix4 &mModel, Texture *texture, const Color &color) override final;
    EXPORT void renderSpriteImage(const Matrix4 &mModel, Texture *texture) override final;

    EXPORT void setAmbientLight(const Color &ambientColor) override final;

    EXPORT bool isAntialiasingMethodAvailable(AntialiasingMethod method) override final;
    EXPORT bool setAntialiasingMethod(AntialiasingMethod method) override final;

    EXPORT void present() override final;

    EXPORT void removeTextureByIndex(unsigned int index) override;
    EXPORT void removeMaterialByIndex(unsigned int index) override;
    EXPORT void removeMeshByIndex(unsigned int index) override;

    EXPORT DirectX9Shader *createDirectX9Shader(const std::string &name, const DWORD *vsoCode, const DWORD *psoCode);

    inline float *getShaderEngineDataPtr() { return engineData; }

protected:
    void initD3D(HWND hWnd, bool bIsFullscreen, int width, int height); // sets up and initializes Direct3D
    void resizeD3D(int width, int height);
    void recreateDevice();
    D3DMULTISAMPLE_TYPE translateAntialiasingType(AntialiasingMethod method);
    void renderQueueDepthBuffer(Camera *camera);
    void renderQueueLightDepthBuffer(Camera *camera);
    void renderQueueDepthEqual(const Vector3 &cameraPosition, Camera *camera);
    void cleanD3D(void);

    // Bonned meshed limited to 4 shadows per mesh
    void setupLights(const Vector3 &objectPosition, float objectRadius);

    inline void setupMaterialColorRender(Material *material)
    {
        Directx9MaterialRenderData *materialRenderData = data.getMaterialRenderData(material);
        if (materialRenderData)
            materialRenderData->setupForRender(&data, bUseRadianceReflection);
    }

    inline void setupMaterialDepthRender(Material *material)
    {
        Directx9MaterialRenderData *materialRenderData = data.getMaterialRenderData(material);
        if (materialRenderData)
            materialRenderData->setupForDepth(&data);
    }

    void renderMeshColorData(Camera *camera, QueuedMeshRenderData *mesh);
    void renderMeshDepthData(Camera *camera, QueuedMeshRenderData *mesh);
    void renderMeshShadowDepthData(Camera *camera, QueuedMeshRenderData *mesh);

    void renderShadowBuffers(const Vector3 &cameraPosition, const Vector3 &cameraFrowardVector);
    void renderShadowBuffersDirectional(const Vector3 &cameraPosition, const Vector3 &cameraFrowardVector, Light *light);
    void renderShadowBuffersSpot(Light *light);

    LPDIRECT3D9 d3d = nullptr;          // the pointer to our Direct3D interface
    LPDIRECT3DDEVICE9 d3ddev = nullptr; // the pointer to the device class

    Mesh *cubeMesh = nullptr;
    Mesh *sphereSkySphere = nullptr;
    Mesh *spriteMesh = nullptr;
    MaterialSimple *lineMaterial = nullptr;
    MaterialSimple *skyMaterial = nullptr;

    // Propageted to shader under c19, common to all. May be used for time, etc
    float engineData[4] = {0, 0, 0, 0};

    // Common textures
    Texture *white = nullptr;
    Texture *black = nullptr;

    // Ambient
    Color ambientColor = Color(1.0f, 1.0f, 1.0f);

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

    // Shadow render of depth
    DirectX9Shader *skyHDRShader = nullptr;

    // Sprite shader
    DirectX9Shader *spriteShader = nullptr;

    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUV = nullptr;
    LPDIRECT3DVERTEXDECLARATION9 pVertexDeclNormalUVSkinned = nullptr;

    Material *defaultMaterial;

    Directx9data data;

    Matrix4 mSpriteViewProjection;

    Texture *radianceTexture = nullptr;
    Texture *ambientTexture = nullptr;

    bool bUseRadianceReflection = false;
    bool bVSync = false;

    Window *window;
};

#endif