// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9renderer.h"
#include "red11.h"
#include "window/windows/windowsWindow.h"
#include <string>
#include <algorithm>

const Color defaultAmbientColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

DirectX9Renderer::DirectX9Renderer(Window *window, bool bVSync) : Renderer(window)
{
    WindowsWindow *winWindow = (WindowsWindow *)window;
    initD3D(winWindow->getHwnd(), false, window->getWidth(), window->getHeight(), bVSync);
}

RendererType DirectX9Renderer::getType()
{
    return RendererType::DirectX9;
}

void DirectX9Renderer::prepareToRender(Texture *ambientTexture, Texture *radianceTexture)
{
    this->ambientTexture = ambientTexture;
    this->radianceTexture = radianceTexture;

    if (viewWidth != window->getWidth() || viewHeight != window->getHeight())
    {
        resizeD3D(window->getWidth(), window->getHeight());
    }

    d3ddev->SetPixelShaderConstantF(19, (const float *)engineData, 1);
}

void DirectX9Renderer::clearBuffer(const Color &color)
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(color.getRedAsUChar(), color.getGreenAsUChar(), color.getBlueAsUChar()), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
}

void DirectX9Renderer::renderCubeMap(Camera *camera, Entity *entity, Texture *hdr)
{
    if (hdr)
    {
        d3ddev->SetRenderState(D3DRS_ZENABLE, false);
        d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
        d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, false);

        Vector3 camPosition = Vector3(entity->getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));

        Entity model;
        model.setPosition(camPosition);
        model.setRotation(Vector3(0, -CONST_PI / 2.0f, 0));

        skyHDRShader->use();

        skyMaterial->setAlbedoColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
        skyMaterial->setEmissionColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
        skyMaterial->setEmissionTexture(hdr);
        setupMaterialColorRender(skyMaterial);

        camera->updateViewMatrix(entity->getModelMatrix());
        d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        renderMesh(camera, sphereSkySphere, &model.getModelMatrix());
        d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }
}

void DirectX9Renderer::queueMesh(Mesh *mesh, Material *material, const Matrix4 *model)
{
    material = material ? material : defaultMaterial;
    data.addMesh(mesh, material, model);
}

void DirectX9Renderer::queueMeshSkinned(Mesh *mesh, Material *material, const Matrix4 *model, std::vector<BoneTransform> *bones)
{
    material = material ? material : defaultMaterial;
    data.addMeshSkinned(mesh, material, model, bones);
}

void DirectX9Renderer::queueLine(const Vector3 &vFrom, const Vector3 &vTo, const Color &color)
{
    data.addLine(vFrom, vTo, color);
}

void DirectX9Renderer::queueLight(Light *light)
{
    data.addLight(light);
}

void DirectX9Renderer::renderQueue(Camera *camera)
{
    Vector3 camPosition = Vector3(*camera->getWorldMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    Vector3 camDirection = camera->getForwardVector();
    camDirection.y = 0;
    camDirection = glm::normalize(camDirection);
    data.recalcDistanceInQueue(camPosition);
    data.remakeActiveQueueForCamera(camera);
    renderShadowBuffers(camPosition, camDirection);
    renderQueueDepthBuffer(camera);
    renderQueueDepthEqual(camPosition, camera);
}

void DirectX9Renderer::clearQueue()
{
    data.clearQueue();
    lastMatrixStore = 0;
}

void DirectX9Renderer::renderMesh(Camera *camera, Mesh *mesh, const Matrix4 *model)
{
    if (!mesh)
        return;
    Directx9MeshRenderData *meshData = data.getMeshRenderData(mesh);

    // === Setup matricies ===
    // View projection for rendering correct positions
    Matrix4 worldViewProjection = *camera->getProjectionMatrix() * *camera->getViewMatrix() * *model;
    worldViewProjection = glm::transpose(worldViewProjection);

    // We don't transpose cause it should be transposed and dx expects matrix to be transposed
    Matrix4 worldInverseTranspose = glm::inverse(*model);

    // Need this one transposed
    Matrix4 worldMatrix = glm::transpose(*model);

    // World matrix or model
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(worldMatrix), 4);

    // World View Projection matrix
    d3ddev->SetVertexShaderConstantF(4, (const float *)value_ptr(worldViewProjection), 4);

    // World Inverse Transpose for normals
    d3ddev->SetVertexShaderConstantF(8, (const float *)value_ptr(worldInverseTranspose), 4);

    // === Render Data and final render ===
    d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUV));
    d3ddev->SetVertexDeclaration(pVertexDeclNormalUV);

    d3ddev->SetIndices(meshData->iBuffer);
    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::renderMeshSkinned(Camera *camera, Mesh *mesh, std::vector<BoneTransform> *bones)
{
    if (!mesh)
        return;
    Directx9MeshRenderData *meshData = data.getMeshRenderData(mesh);

    // set bone matrices, base register is 32, so, 56 bone is availalbe
    for (auto &bone : *bones)
    {
        int reg = 32 + bone.deform->index * 4;
        if (reg >= 256)
            break;
        Matrix4 boneMatrix = *bone.model * bone.deform->getInvBindMatrix();
        boneMatrix = glm::transpose(boneMatrix);
        d3ddev->SetVertexShaderConstantF(reg, (const float *)value_ptr(boneMatrix), 4);
    }

    // Shader multiplies model matrix internally, so view projection needs to be provcided
    Matrix4 viewProjection = glm::transpose(*camera->getProjectionMatrix() * *camera->getViewMatrix());
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(viewProjection), 4);

    // === Render Data and final render ===
    d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUVSkinned));
    d3ddev->SetVertexDeclaration(pVertexDeclNormalUVSkinned);

    d3ddev->SetIndices(meshData->iBuffer);
    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::renderLine(Camera *camera, const Vector3 &vFrom, const Vector3 &vTo)
{
    Vector3 center = (vFrom + vTo) * 0.5f;
    Vector3 difference = vTo - vFrom;
    Vector3 normal = glm::normalize(difference);

    float y = atan2f(normal.z, normal.x);
    float len = sqrtf(normal.x * normal.x + normal.z * normal.z);
    float x = atan2(len, normal.y);

    Entity entity;

    entity.setPosition(center);
    entity.setScale(Vector3(0.0008f, 0.0008f, glm::length(difference)));
    entity.setRotation(Vector3(CONST_PI / 2 - x, -y - CONST_PI / 2.0f, 0.0f));

    renderMesh(camera, cubeMesh, &entity.getModelMatrix());
}

void DirectX9Renderer::setupSpriteRendering(const Matrix4 &mView, const Matrix4 &mProjection)
{
    mSpriteViewProjection = mProjection * mView;

    d3ddev->SetRenderState(D3DRS_ZENABLE, false);
    d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, false);

    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    spriteShader->use();

    d3ddev->SetVertexDeclaration(pVertexDeclNormalUV);
}

void DirectX9Renderer::renderSpriteRect(const Matrix4 &mModel, const Color &color)
{
    Matrix4 worldViewProjection = glm::transpose(mSpriteViewProjection * mModel);
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(worldViewProjection), 4);

    Directx9MeshRenderData *meshData = data.getMeshRenderData(spriteMesh);

    // Sprite shader settings
    // UV add X, UV add Y, UV div x, UV div y
    float vertexShaderSettings[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    d3ddev->SetVertexShaderConstantF(4, (const float *)vertexShaderSettings, 1);

    // Color
    d3ddev->SetPixelShaderConstantF(0, (const float *)color.getAsFloatArray(), 1);

    // Pixel shader settings
    // Use texture, As mask, *, *
    float pixelShaderSettings[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    d3ddev->SetPixelShaderConstantF(1, (const float *)pixelShaderSettings, 1);

    // === Render Data and final render ===
    d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUV));
    d3ddev->SetIndices(meshData->iBuffer);

    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::renderSpriteMask(const Matrix4 &mModel, Texture *texture, const Color &color)
{
    Matrix4 worldViewProjection = glm::transpose(mSpriteViewProjection * mModel);
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(worldViewProjection), 4);

    Directx9MeshRenderData *meshData = data.getMeshRenderData(spriteMesh);

    // Sprite shader settings
    // UV add X, UV add Y, UV div x, UV div y
    float vertexShaderSettings[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    d3ddev->SetVertexShaderConstantF(4, (const float *)vertexShaderSettings, 1);

    // Color
    d3ddev->SetPixelShaderConstantF(0, (const float *)color.getAsFloatArray(), 1);

    // Pixel shader settings
    // Use texture, As mask, *, *
    float pixelShaderSettings[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    d3ddev->SetPixelShaderConstantF(1, (const float *)pixelShaderSettings, 1);

    // Mask
    Directx9TextureRenderData *maskTextureData = data.getTextureRenderData(texture);
    if (maskTextureData)
        d3ddev->SetTexture(0, maskTextureData->texture);

    // === Render Data and final render ===
    d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUV));
    d3ddev->SetIndices(meshData->iBuffer);

    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::renderSpriteImage(const Matrix4 &mModel, Texture *texture)
{
    Matrix4 worldViewProjection = glm::transpose(mSpriteViewProjection * mModel);
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(worldViewProjection), 4);

    Directx9MeshRenderData *meshData = data.getMeshRenderData(spriteMesh);

    // Sprite shader settings
    // UV add X, UV add Y, UV div x, UV div y
    float vertexShaderSettings[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    d3ddev->SetVertexShaderConstantF(4, (const float *)vertexShaderSettings, 1);

    // Pixel shader settings
    // Use texture, As mask, *, *
    float pixelShaderSettings[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    d3ddev->SetPixelShaderConstantF(1, (const float *)pixelShaderSettings, 1);

    // Mask
    Directx9TextureRenderData *maskTextureData = data.getTextureRenderData(texture);
    if (maskTextureData)
        d3ddev->SetTexture(0, maskTextureData->texture);

    // === Render Data and final render ===
    d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUV));
    d3ddev->SetIndices(meshData->iBuffer);

    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::setAmbientLight(const Color &ambientColor)
{
    this->ambientColor = ambientColor;
}

void DirectX9Renderer::present()
{
    d3ddev->Present(NULL, NULL, NULL, NULL);
}

void DirectX9Renderer::removeTextureByIndex(unsigned int index)
{
    data.destroyTextureRenderDataByIndex(index);
}

void DirectX9Renderer::removeMaterialByIndex(unsigned int index)
{
    data.destroyMaterialRenderDataByIndex(index);
}

void DirectX9Renderer::removeMeshByIndex(unsigned int index)
{
    data.destroyMeshRenderDataByIndex(index);
}

DirectX9Shader *DirectX9Renderer::createDirectX9Shader(const std::string &name, const DWORD *vsoCode, const DWORD *psoCode)
{
    return new DirectX9Shader(name, d3ddev, vsoCode, psoCode);
}

// function prototypes
void DirectX9Renderer::initD3D(HWND hWnd, bool bIsFullscreen, int width, int height, bool bVSync)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

    int displayWidth = GetSystemMetrics(SM_CXSCREEN);
    int displayHeight = GetSystemMetrics(SM_CYSCREEN);

    D3DPRESENT_PARAMETERS d3dpp;       // create a struct to hold various device information
    ZeroMemory(&d3dpp, sizeof(d3dpp)); // clear out the struct for use
    d3dpp.Windowed = !bIsFullscreen;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    d3dpp.hDeviceWindow = hWnd;               // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = displayWidth;     // set the width of the buffer
    d3dpp.BackBufferHeight = displayHeight;   // set the height of the buffer
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.PresentationInterval = bVSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
                      &d3dpp,
                      &d3ddev);

    data.d3ddev = d3ddev;

    cubeMesh = Red11::getMeshBuilder()->createCube(1.0f);
    cubeMesh->addUser();
    spriteMesh = Red11::getMeshBuilder()->createSprite(1.0f);
    spriteMesh->addUser();
    sphereSkySphere = Red11::getMeshBuilder()->createSphere(1.0f, 14, 10);
    sphereSkySphere->addUser();

    lineMaterial = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.2, 1.0, 0.2));
    lineMaterial->addUser();
    skyMaterial = new MaterialSimple(Color(0.0, 0.0, 0.0));
    skyMaterial->addUser();

    UVSimpleShader = new DirectX9Shader("UV Simple Shader", d3ddev, (const DWORD *)UVSimpleVertexShader_vso, (const DWORD *)UVSimpleFragmentShader_pso);
    UVSimpleMaskShader = new DirectX9Shader("UV Simple Mask Shader", d3ddev, (const DWORD *)UVSimpleMaskVertexShader_vso, (const DWORD *)UVSimpleMaskFragmentShader_pso);
    UVShader = new DirectX9Shader("UV Shader", d3ddev, (const DWORD *)UVVertexShader_vso, (const DWORD *)UVFragmentShader_pso);
    UVNormalShader = new DirectX9Shader("UV Normal Shader", d3ddev, (const DWORD *)UVNormalVertexShader_vso, (const DWORD *)UVNormalFragmentShader_pso);
    UVSkinnedShader = new DirectX9Shader("UV Skinned Shader", d3ddev, (const DWORD *)UVSkinnedVertexShader_vso, (const DWORD *)UVFragmentShader_pso);
    UVSkinnedNormalShader = new DirectX9Shader("UV Skinned Normal Shader", d3ddev, (const DWORD *)UVSkinnedNormalVertexShader_vso, (const DWORD *)UVNormalFragmentShader_pso);
    UVShadowShader = new DirectX9Shader("UV Shadow Shader", d3ddev, (const DWORD *)UVShadowVertexShader_vso, (const DWORD *)UVShadowFragmentShader_pso);
    UVShadowMaskShader = new DirectX9Shader("UV Shadow Mask Shader", d3ddev, (const DWORD *)UVShadowMaskVertexShader_vso, (const DWORD *)UVShadowMaskFragmentShader_pso);
    UVShadowSkinnedShader = new DirectX9Shader("UV Shadow Skinned Shader", d3ddev, (const DWORD *)UVShadowSkinnedVertexShader_vso, (const DWORD *)UVShadowFragmentShader_pso);
    skyHDRShader = new DirectX9Shader("Sky HDR Shader", d3ddev, (const DWORD *)skyHDRVertexShader_vso, (const DWORD *)skyHDRFragmentShader_pso);
    spriteShader = new DirectX9Shader("UV Normal Shader", d3ddev, (const DWORD *)SpriteVertexShader_vso, (const DWORD *)SpriteFragmentShader_pso);

    D3DVERTEXELEMENT9 VertexElementsNormalUV[] = {
        {0, offsetof(DX9VertexNormalUV, x), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(DX9VertexNormalUV, normalX), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(DX9VertexNormalUV, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, offsetof(DX9VertexNormalUV, tangent), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
        {0, offsetof(DX9VertexNormalUV, bitangent), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
        D3DDECL_END()};

    D3DVERTEXELEMENT9 VertexElementsNormalUVSkinned[] = {
        {0, offsetof(DX9VertexNormalUVSkinned, x), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, normalX), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, tangent), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, bitangent), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, boneIndices), D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, boneWeights), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        D3DDECL_END()};

    d3ddev->CreateVertexDeclaration(VertexElementsNormalUV, &pVertexDeclNormalUV);
    d3ddev->CreateVertexDeclaration(VertexElementsNormalUVSkinned, &pVertexDeclNormalUVSkinned);

    defaultMaterial = new MaterialSimple(Color(0.6f, 0.6f, 0.6f));
    defaultMaterial->addUser();

    // shadow slots setup
    for (int i = 10; i < 16; i++)
    {
        d3ddev->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
        d3ddev->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
        d3ddev->SetSamplerState(i, D3DSAMP_BORDERCOLOR, D3DCOLOR_XRGB(255, 255, 255));
    }

    // filtering setup
    for (int i = 0; i < 16; i++)
    {
        d3ddev->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        d3ddev->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        d3ddev->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    }

    // basic textures
    unsigned char whiteData[4] = {255, 255, 255, 255};
    unsigned char blackData[4] = {0, 0, 0, 255};
    white = new Texture("WhiteTexture", TextureType::Normal, 1, 1, whiteData);
    white->addUser();
    black = new Texture("BlackTexture", TextureType::Normal, 1, 1, blackData);
    black->addUser();
}

void DirectX9Renderer::resizeD3D(int width, int height)
{
    this->viewWidth = width;
    this->viewHeight = height;
}

void DirectX9Renderer::renderQueueDepthBuffer(Camera *camera)
{
    // Enable z buffer
    d3ddev->SetRenderState(D3DRS_ZENABLE, true);
    // Disable writing to the color buffer
    d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
    // Enable depth writing
    d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, true);
    // Set the depth comparison function (default is D3DCMP_LESSEQUAL)
    d3ddev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

    d3ddev->BeginScene();
    for (int i = 0; i < data.queueActiveCurrentMesh; i++)
    {
        if (!data.queueActiveMeshes[i]->material->isAlphaPhase())
            renderMeshDepthData(camera, data.queueActiveMeshes[i]);
    }
    d3ddev->EndScene();
}

void DirectX9Renderer::renderQueueLightDepthBuffer(Camera *camera)
{
    // Enable z buffer
    d3ddev->SetRenderState(D3DRS_ZENABLE, true);
    // Disable writing to the color buffer
    d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    // Enable depth writing
    d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, true);
    // Set the depth comparison function (default is D3DCMP_LESSEQUAL)
    d3ddev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

    d3ddev->BeginScene();
    for (int i = 0; i < data.queueCurrentMesh; i++)
    {
        if (data.queueMeshes[i].mesh->isCastsShadow() && data.isMeshVisibleToCamera(&data.queueMeshes[i], camera))
            renderMeshShadowDepthData(camera, &data.queueMeshes[i]);
    }
    d3ddev->EndScene();
}

void DirectX9Renderer::renderQueueDepthEqual(const Vector3 &cameraPosition, Camera *camera)
{
    // Enable z buffer
    d3ddev->SetRenderState(D3DRS_ZENABLE, true);
    // Enable writing to the color buffer
    d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    // Enable depth writing
    d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, true);
    // Set the depth equal comparison function
    d3ddev->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);

    // Ambient Color is shared
    d3ddev->SetPixelShaderConstantF(18, ambientColor.getAsFloatArray(), 1);

    if (ambientTexture && radianceTexture)
    {
        bUseRadianceReflection = true;
        Directx9TextureRenderData *radianceTextureData = data.getTextureRenderData(radianceTexture);
        Directx9TextureRenderData *reflectionTextureData = data.getTextureRenderData(ambientTexture);

        if (radianceTextureData && reflectionTextureData)
        {
            d3ddev->SetTexture(8, radianceTextureData->texture);
            d3ddev->SetTexture(9, reflectionTextureData->texture);
        }
    }
    else
    {
        bUseRadianceReflection = false;
    }

    // Camera position is shared among all render targets
    d3ddev->SetPixelShaderConstantF(17, (const float *)value_ptr(cameraPosition), 1);

    std::vector<QueuedMeshRenderData *> alphaRenderMeshes;

    d3ddev->BeginScene();

    // === Render Solid Meshes ===
    for (int i = 0; i < data.queueActiveCurrentMesh; i++)
    {
        if (data.queueActiveMeshes[i]->material->isAlphaPhase())
            alphaRenderMeshes.push_back(data.queueActiveMeshes[i]);
        else
            renderMeshColorData(camera, data.queueActiveMeshes[i]);
    }

    // === Sort and Render Alpha Meshes ===
    d3ddev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, false);
    std::sort(alphaRenderMeshes.begin(), alphaRenderMeshes.end(), [](const QueuedMeshRenderData *a, const QueuedMeshRenderData *b)
              { return a->distance > b->distance; });
    for (auto &renderMesh : alphaRenderMeshes)
    {
        renderMeshColorData(camera, renderMesh);
    }

    if (data.queueCurrentLine > 0)
    {
        d3ddev->SetRenderState(D3DRS_ZENABLE, false);
        setupMaterialColorRender(lineMaterial);
        for (int i = 0; i < data.queueCurrentLine; i++)
        {
            // Emission Color
            Vector4 color = Vector4(data.queueLines[i].color.r, data.queueLines[i].color.g, data.queueLines[i].color.b, 1.0f);
            d3ddev->SetPixelShaderConstantF(16, (const float *)value_ptr(color), 1);

            UVShader->use();

            renderLine(camera, data.queueLines[i].vFrom, data.queueLines[i].vTo);
        }
    }

    d3ddev->EndScene();
}

void DirectX9Renderer::cleanD3D(void)
{
    if (d3ddev)
    {
        d3ddev->Release(); // close and release the 3D device
        d3ddev = nullptr;
    }
    if (d3d)
    {
        d3d->Release(); // close and release Direct3D
        d3d = nullptr;
    }
}

void DirectX9Renderer::setupLights(const Vector3 &objectPosition, float objectRadius)
{
    // registers for lights:
    // 20 - 84 - light data
    // 16 - 32 - vertex shadow light matricies
    // type, typeData, typeData, typeData
    // positionv3, xxx
    // normalv3, power
    // colorv3, radius

    std::vector<DX9AffectingLight> affectingLights;
    for (int lightNum = 0; lightNum < data.queueActiveCurrentLight; lightNum++)
    {
        Light *lightData = data.queueActiveLights[lightNum]->light;
        float distance = lightData->isAffecting(objectPosition, objectRadius);
        if (distance > 0.0f)
            affectingLights.push_back({distance, lightData});
    }

    std::sort(affectingLights.begin(), affectingLights.end(), [](const DX9AffectingLight a, const DX9AffectingLight b)
              { return (b.distance > a.distance); });

    if (affectingLights.size() > MAX_LIGHTS_PER_MESH_COUNT)
        affectingLights.resize(MAX_LIGHTS_PER_MESH_COUNT);

    // Bonned meshed limited to 4 shadows per mesh
    int baseReg = 20;
    int shadowMatrixBaseReg = 16;
    int shadowMatrixMaxReg = 28;
    int shadowTextureBaseReg = 10;
    DX9LightShaderStruct dxLight;
    memset(&dxLight, 0, sizeof(DX9LightShaderStruct));

    for (int i = 0; i < MAX_LIGHTS_PER_MESH_COUNT; i++)
    {
        auto lightData = i < affectingLights.size() ? &affectingLights.at(i) : nullptr;
        auto light = lightData ? lightData->light : nullptr;
        if (!light)
        {
            dxLight.type = 0.0f;
            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 1);
        }
        else if (light->getType() == LightType::Directional)
        {
            Color lightColor = light->getColor();
            Vector3 lightDirection = light->getNormal();
            bool castShadow = light->isShadowsEnabled() && (shadowMatrixBaseReg < shadowMatrixMaxReg);

            dxLight.type = 1.0f;
            dxLight.castShadow = castShadow ? 1.0f : 0.0f;
            dxLight.texelSize = light->getShadowTextureTexelSize();
            dxLight.attConstant = 1.0f;
            dxLight.attLinear = 1.0f;
            dxLight.attQuadratic = 1.0f;
            dxLight.innerRadius = 1.0f;

            dxLight.normal[0] = lightDirection.x;
            dxLight.normal[1] = lightDirection.y;
            dxLight.normal[2] = lightDirection.z;

            dxLight.color[0] = lightColor.r;
            dxLight.color[1] = lightColor.g;
            dxLight.color[2] = lightColor.b;

            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 4);

            if (castShadow)
            {
                for (int c = 0; c < light->getNumOfCascades(); c++)
                {
                    Directx9TextureRenderData *cascade = data.getTextureRenderData(light->getShadowTexture(c));
                    if (cascade)
                    {
                        d3ddev->SetTexture(shadowTextureBaseReg + c, cascade->texture);
                    }
                }
                d3ddev->SetVertexShaderConstantF(shadowMatrixBaseReg, (const float *)value_ptr(light->getShadowViewProjectionMatrix()), 4);
            }
        }
        else if (light->getType() == LightType::Omni)
        {
            Color lightColor = light->getColor();
            Vector3 lightPosition = light->getPosition();

            dxLight.type = 2.0f;
            dxLight.castShadow = 0.0f;
            dxLight.texelSize = 0.0f;
            dxLight.attConstant = light->getAttenuation().constant;
            dxLight.attLinear = light->getAttenuation().linear;
            dxLight.attQuadratic = light->getAttenuation().quadratic;
            dxLight.innerRadius = 0.0f;

            dxLight.position[0] = lightPosition.x;
            dxLight.position[1] = lightPosition.y;
            dxLight.position[2] = lightPosition.z;

            dxLight.color[0] = lightColor.r;
            dxLight.color[1] = lightColor.g;
            dxLight.color[2] = lightColor.b;

            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 4);
        }
        else if (light->getType() == LightType::Spot)
        {
            Color lightColor = light->getColor();
            Vector3 lightDirection = light->getNormal();
            Vector3 lightPosition = light->getPosition();
            bool castShadow = light->isShadowsEnabled() && (shadowMatrixBaseReg < shadowMatrixMaxReg);

            dxLight.type = 3.0f;
            dxLight.castShadow = castShadow ? 1.0f : 0.0f;
            dxLight.texelSize = cosf(light->getRadius());
            dxLight.innerRadius = cosf(light->getInnerRadius());
            dxLight.attConstant = light->getAttenuation().constant;
            dxLight.attLinear = light->getAttenuation().linear;
            dxLight.attQuadratic = light->getAttenuation().quadratic;

            dxLight.position[0] = lightPosition.x;
            dxLight.position[1] = lightPosition.y;
            dxLight.position[2] = lightPosition.z;

            dxLight.normal[0] = lightDirection.x;
            dxLight.normal[1] = lightDirection.y;
            dxLight.normal[2] = lightDirection.z;

            dxLight.color[0] = lightColor.r;
            dxLight.color[1] = lightColor.g;
            dxLight.color[2] = lightColor.b;

            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 4);

            if (castShadow)
            {
                Directx9TextureRenderData *shadowTexture = data.getTextureRenderData(light->getShadowTexture(0));
                if (shadowTexture)
                {
                    Directx9TextureRenderData *shadowMaskTexture = data.getTextureRenderData(light->getShadowMaskTexture());
                    if (!shadowMaskTexture)
                        shadowMaskTexture = data.getTextureRenderData(white);

                    d3ddev->SetTexture(shadowTextureBaseReg, shadowTexture->texture);
                    if (shadowMaskTexture)
                        d3ddev->SetTexture(shadowTextureBaseReg + 1, shadowMaskTexture->texture);
                }
                d3ddev->SetVertexShaderConstantF(shadowMatrixBaseReg, (const float *)value_ptr(light->getShadowViewProjectionMatrix()), 4);
            }
        }
        baseReg += 4;
        shadowMatrixBaseReg += 4;
        shadowTextureBaseReg += 2;
    }
}

void DirectX9Renderer::renderMeshColorData(Camera *camera, QueuedMeshRenderData *mesh)
{
    setupLights(Vector3(*mesh->model * Vector4(mesh->centroid, 1.0f)), 1.0f);
    setupMaterialColorRender(mesh->material);

    if (mesh->bones.size() > 0)
    {
        if (mesh->material->getShaderColorSkinned(RendererType::DirectX9))
            mesh->material->getShaderColorSkinned(RendererType::DirectX9)->use();
        else
        {
            if (mesh->material->isUsingNormalMap())
                UVSkinnedNormalShader->use();
            else
                UVSkinnedShader->use();
        }
        renderMeshSkinned(camera, mesh->mesh, &mesh->bones);
    }
    else
    {
        if (mesh->material->getShaderColor(RendererType::DirectX9))
            mesh->material->getShaderColor(RendererType::DirectX9)->use();
        else
        {
            if (mesh->material->isUsingNormalMap())
                UVNormalShader->use();
            else
                UVShader->use();
        }
        renderMesh(camera, mesh->mesh, mesh->model);
    }
}

void DirectX9Renderer::renderMeshDepthData(Camera *camera, QueuedMeshRenderData *mesh)
{
    if (mesh->bones.size() > 0)
    {
        setupMaterialDepthRender(mesh->material);
        if (mesh->material->getShaderDepthSkinned(RendererType::DirectX9))
            mesh->material->getShaderDepthSkinned(RendererType::DirectX9)->use();
        else
            UVSkinnedShader->use();
        renderMeshSkinned(camera, mesh->mesh, &mesh->bones);
    }
    else
    {
        if (mesh->material->getDisplay() == MaterialDisplay::SolidMask)
        {
            setupMaterialDepthRender(mesh->material);
            if (mesh->material->getShaderDepth(RendererType::DirectX9))
                mesh->material->getShaderDepth(RendererType::DirectX9)->use();
            else
                UVSimpleMaskShader->use();
        }
        else
        {
            if (mesh->material->getShaderDepth(RendererType::DirectX9))
                mesh->material->getShaderDepth(RendererType::DirectX9)->use();
            else
                UVSimpleShader->use();
        }

        renderMesh(camera, mesh->mesh, mesh->model);
    }
}

void DirectX9Renderer::renderMeshShadowDepthData(Camera *camera, QueuedMeshRenderData *mesh)
{
    if (mesh->bones.size() > 0)
    {
        if (mesh->material->getShaderShadowSkinned(RendererType::DirectX9))
            mesh->material->getShaderShadowSkinned(RendererType::DirectX9)->use();
        else
            UVShadowSkinnedShader->use();
        renderMeshSkinned(camera, mesh->mesh, &mesh->bones);
    }
    else
    {
        if (mesh->material->getDisplay() == MaterialDisplay::Solid)
        {
            if (mesh->material->getShaderShadow(RendererType::DirectX9))
                mesh->material->getShaderShadow(RendererType::DirectX9)->use();
            else
                UVShadowShader->use();
        }
        else
        {
            setupMaterialDepthRender(mesh->material);
            if (mesh->material->getShaderShadow(RendererType::DirectX9))
                mesh->material->getShaderShadow(RendererType::DirectX9)->use();
            else
                UVSimpleMaskShader->use();
        }

        renderMesh(camera, mesh->mesh, mesh->model);
    }
}

void DirectX9Renderer::renderShadowBuffers(const Vector3 &cameraPosition, const Vector3 &cameraFrowardVector)
{
    for (int i = 0; i < data.queueActiveCurrentLight; i++)
    {
        if (data.queueActiveLights[i]->light && data.queueActiveLights[i]->light->isShadowsEnabled())
        {
            if (data.queueActiveLights[i]->light->getType() == LightType::Directional)
                renderShadowBuffersDirectional(cameraPosition, cameraFrowardVector, data.queueActiveLights[i]->light);
            if (data.queueActiveLights[i]->light->getType() == LightType::Spot)
                renderShadowBuffersSpot(data.queueActiveLights[i]->light);
        }
    }
}

void DirectX9Renderer::renderShadowBuffersDirectional(const Vector3 &cameraPosition, const Vector3 &cameraFrowardVector, Light *light)
{
    IDirect3DSurface9 *originalRenderTarget = NULL;
    IDirect3DSurface9 *originalDepthStencil = NULL;
    Camera camera;
    Entity cameraEntity;

    d3ddev->GetRenderTarget(0, &originalRenderTarget);
    d3ddev->GetDepthStencilSurface(&originalDepthStencil);

    for (int i = 0; i < light->getNumOfCascades(); i++)
    {
        float cascadeSize = light->getCascadeDistance() * float(i + 1);

        Directx9TextureRenderData *cascade = data.getTextureRenderData(light->getShadowTexture(i));
        if (!cascade)
            continue;

        Vector3 lightShotPosition = cameraPosition + light->getNormal() * cascadeSize * -8.0f;
        camera.setupAsOrthographic(cascadeSize, cascadeSize, 0.0f, cascadeSize * 12.0f);
        cameraEntity.setPosition(lightShotPosition);
        cameraEntity.setRotationAlongNormal(light->getNormal(), Vector3(0.0f, 0.0f, -1.0f));
        camera.updateViewMatrix(cameraEntity.getModelMatrix());

        d3ddev->SetRenderTarget(0, cascade->surface);
        d3ddev->SetDepthStencilSurface(cascade->depthDataSurface);

        d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
        d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        renderQueueLightDepthBuffer(&camera);
        if (i == 0)
            light->setShadowViewProjectionMatrix(glm::transpose(*camera.getProjectionMatrix() * *camera.getViewMatrix()));
    }

    d3ddev->SetRenderTarget(0, originalRenderTarget);
    d3ddev->SetDepthStencilSurface(originalDepthStencil);
}

void DirectX9Renderer::renderShadowBuffersSpot(Light *light)
{
    Directx9TextureRenderData *shadowData = data.getTextureRenderData(light->getShadowTexture(0));
    if (!shadowData)
        return;

    IDirect3DSurface9 *originalRenderTarget = NULL;
    IDirect3DSurface9 *originalDepthStencil = NULL;
    Camera camera;
    Entity cameraEntity;

    Vector3 &lightPosition = light->getPosition();
    Vector3 &lightNormal = light->getNormal();
    float outerRadius = light->getRadius();

    cameraEntity.setPosition(lightPosition);
    cameraEntity.setRotationAlongNormal(-lightNormal, Vector3(0.0f, 0.0f, -1.0f));

    d3ddev->GetRenderTarget(0, &originalRenderTarget);
    d3ddev->GetDepthStencilSurface(&originalDepthStencil);

    camera.setupAsPerspective(light->getBufferSize(), light->getBufferSize(), 0.05f, 16.0f, outerRadius * 2.0f);
    camera.updateViewMatrix(cameraEntity.getModelMatrix());

    d3ddev->SetRenderTarget(0, shadowData->surface);
    d3ddev->SetDepthStencilSurface(shadowData->depthDataSurface);

    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    renderQueueLightDepthBuffer(&camera);

    light->setShadowViewProjectionMatrix(glm::transpose(*camera.getProjectionMatrix() * *camera.getViewMatrix()));

    d3ddev->SetRenderTarget(0, originalRenderTarget);
    d3ddev->SetDepthStencilSurface(originalDepthStencil);
}