// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9renderer.h"
#include "red11.h"
#include "window/windows/windowsWindow.h"
#include <string>
#include <algorithm>

DirectX9Renderer::DirectX9Renderer(Window *window) : Renderer(window)
{
    WindowsWindow *winWindow = (WindowsWindow *)window;
    initD3D(winWindow->getHwnd(), winWindow->isFullscreen(), window->getWidth(), window->getHeight());
}

RendererType DirectX9Renderer::getType()
{
    return RendererType::DirectX9;
}

void DirectX9Renderer::clearBuffer(Color color)
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(color.getRedAsUChar(), color.getGreenAsUChar(), color.getBlueAsUChar()), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
}

void DirectX9Renderer::queueMesh(Mesh *mesh, Material *material, Matrix4 *model)
{
    material = material ? material : defaultMaterial;
    data.addMesh(mesh, material, model);
}

void DirectX9Renderer::queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones)
{
    material = material ? material : defaultMaterial;
    data.addMeshSkinned(mesh, material, model, bones);
}

void DirectX9Renderer::queueLine(Vector3 vFrom, Vector3 vTo, Color color)
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
    renderQueueDepthBuffer(camPosition, camera);
    renderQueueDepthEqual(camPosition, camera);
}

void DirectX9Renderer::clearQueue()
{
    data.clearQueue();
}

void DirectX9Renderer::renderMesh(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model)
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

void DirectX9Renderer::renderMeshSkinned(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model, std::vector<BoneTransform> *bones)
{
    if (!mesh)
        return;
    Directx9MeshRenderData *meshData = data.getMeshRenderData(mesh);

    // set bone matrices, base register is 60, so, 49 bone is availalbe
    for (auto &bone : *bones)
    {
        Deform *deform = mesh->getDeformByName(*bone.name);
        if (deform)
        {
            int reg = 60 + deform->index * 4;
            Matrix4 boneMatrix = *bone.model * deform->getInvBindMatrix();
            boneMatrix = glm::transpose(boneMatrix);
            d3ddev->SetVertexShaderConstantF(reg, (const float *)value_ptr(boneMatrix), 4);
        }
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

void DirectX9Renderer::renderLine(Camera *camera, Vector3 vFrom, Vector3 vTo)
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

    Vector3 camPosition = Vector3(*camera->getWorldMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    renderMesh(camera, &camPosition, cubeMesh, entity.getModelMatrix());
}

void DirectX9Renderer::setAmbientLight(Color &ambientColor)
{
    this->ambientColor = ambientColor;
}

void DirectX9Renderer::present()
{
    d3ddev->Present(NULL, NULL, NULL, NULL);
}

// function prototypes
void DirectX9Renderer::initD3D(HWND hWnd, bool bIsFullscreen, int width, int height)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;       // create a struct to hold various device information
    ZeroMemory(&d3dpp, sizeof(d3dpp)); // clear out the struct for use
    d3dpp.Windowed = !bIsFullscreen;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    d3dpp.hDeviceWindow = hWnd;               // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = width;            // set the width of the buffer
    d3dpp.BackBufferHeight = height;          // set the height of the buffer
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // create a device class using this information and information from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
                      &d3dpp,
                      &d3ddev);

    data.d3ddev = d3ddev;

    cubeMesh = Red11::getMeshBuilder()->createCube(1.0f);
    lineMaterial = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.2, 1.0, 0.2));

    if (d3ddev->CreateVertexShader((const DWORD *)UVSimpleVertexShader_vso, &pUVSimpleVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVSimpleFragmentShader_pso, &pUVSimpleFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVSimpleMaskVertexShader_vso, &pUVSimpleMaskVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVSimpleMaskFragmentShader_pso, &pUVSimpleMaskFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVVertexShader_vso, &pUVVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVFragmentShader_pso, &pUVFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVNormalVertexShader_vso, &pUVNormalVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVNormalFragmentShader_pso, &pUVNormalFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVSkinnedVertexShader_vso, &pUVSkinnedVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVSkinnedFragmentShader_pso, &pUVSkinnedFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVSkinnedNormalVertexShader_vso, &pUVSkinnedNormalVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVSkinnedNormalFragmentShader_pso, &pUVSkinnedNormalFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVShadowVertexShader_vso, &pUVShadowVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVShadowFragmentShader_pso, &pUVShadowFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVShadowMaskVertexShader_vso, &pUVShadowMaskVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVShadowMaskFragmentShader_pso, &pUVShadowMaskFragmentShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)UVShadowSkinnedVertexShader_vso, &pUVShadowSkinnedVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");

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

    // shadow slots setup
    for (int i = 8; i < 16; i++)
    {
        d3ddev->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
        d3ddev->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
        d3ddev->SetSamplerState(i, D3DSAMP_BORDERCOLOR, D3DCOLOR_XRGB(255, 255, 255));
        d3ddev->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        d3ddev->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        d3ddev->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    }
}

void DirectX9Renderer::renderQueueDepthBuffer(Vector3 &cameraPosition, Camera *camera)
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
            renderMeshDepthData(camera, cameraPosition, data.queueActiveMeshes[i]);
    }
    d3ddev->EndScene();
}

void DirectX9Renderer::renderQueueLightDepthBuffer(Vector3 &cameraPosition, Camera *camera)
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
        if (data.isMeshVisibleToCamera(&data.queueMeshes[i], camera))
            renderMeshShadowDepthData(camera, cameraPosition, &data.queueMeshes[i]);
    }
    d3ddev->EndScene();
}

void DirectX9Renderer::renderQueueDepthEqual(Vector3 &cameraPosition, Camera *camera)
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
            renderMeshColorData(camera, cameraPosition, data.queueActiveMeshes[i]);
    }

    // === Sort and Render Alpha Meshes ===
    d3ddev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    std::sort(alphaRenderMeshes.begin(), alphaRenderMeshes.end(), [](const QueuedMeshRenderData *a, const QueuedMeshRenderData *b)
              { return a->distance > b->distance; });
    for (auto &renderMesh : alphaRenderMeshes)
    {
        renderMeshColorData(camera, cameraPosition, renderMesh);
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

            d3ddev->SetVertexShader(pUVVertexShader);
            d3ddev->SetPixelShader(pUVFragmentShader);

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

void DirectX9Renderer::setupLights(Vector3 objectPosition, float objectRadius)
{
    // registers for lights:
    // 20 - 52 - light data
    // 52 - 60 - light matricies
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

    int baseReg = 20;
    int shadowMatrixBaseReg = 52;
    int shadowTextureBaseReg = 8;
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
            bool castShadow = light->isShadowsEnabled() && (shadowMatrixBaseReg < 60);

            dxLight.type = 1.0f;
            dxLight.castShadow = castShadow ? 1.0f : 0.0f;
            dxLight.texelSize = light->getShadowTextureTexelSize();
            dxLight.data0 = 1.0f;
            dxLight.data1 = 1.0f;
            dxLight.data2 = 1.0f;
            dxLight.data3 = 1.0f;

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
            dxLight.data0 = light->getAttenuation().constant;
            dxLight.data1 = light->getAttenuation().linear;
            dxLight.data2 = light->getAttenuation().quadratic;
            dxLight.data3 = 0.0f;

            dxLight.position[0] = lightPosition.x;
            dxLight.position[1] = lightPosition.y;
            dxLight.position[2] = lightPosition.z;

            dxLight.color[0] = lightColor.r;
            dxLight.color[1] = lightColor.g;
            dxLight.color[2] = lightColor.b;

            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 4);
        }
        baseReg += 4;
        shadowMatrixBaseReg += 4;
        shadowTextureBaseReg += 2;
    }
}

void DirectX9Renderer::renderMeshColorData(Camera *camera, Vector3 &cameraPosition, QueuedMeshRenderData *mesh)
{
    setupLights(Vector3(*mesh->model * Vector4(mesh->centroid, 1.0f)), 1.0f);
    setupMaterialColorRender(mesh->material);

    if (mesh->bones)
    {
        if (mesh->material->isUsingNormalMap())
        {
            d3ddev->SetVertexShader(pUVSkinnedNormalVertexShader);
            d3ddev->SetPixelShader(pUVSkinnedNormalFragmentShader);
        }
        else
        {
            d3ddev->SetVertexShader(pUVSkinnedVertexShader);
            d3ddev->SetPixelShader(pUVSkinnedFragmentShader);
        }

        renderMeshSkinned(camera, &cameraPosition, mesh->mesh, mesh->model, mesh->bones);
    }
    else
    {
        if (mesh->material->isUsingNormalMap())
        {
            // normals require some additional caluclations, done for performance
            d3ddev->SetVertexShader(pUVNormalVertexShader);
            d3ddev->SetPixelShader(pUVNormalFragmentShader);
        }
        else
        {
            d3ddev->SetVertexShader(pUVVertexShader);
            d3ddev->SetPixelShader(pUVFragmentShader);
        }
        renderMesh(camera, &cameraPosition, mesh->mesh, mesh->model);
    }
}

void DirectX9Renderer::renderMeshDepthData(Camera *camera, Vector3 &cameraPosition, QueuedMeshRenderData *mesh)
{
    if (mesh->bones)
    {
        d3ddev->SetVertexShader(pUVSkinnedVertexShader);
        d3ddev->SetPixelShader(pUVSkinnedFragmentShader);

        renderMeshSkinned(camera, &cameraPosition, mesh->mesh, mesh->model, mesh->bones);
    }
    else
    {
        if (mesh->material->getDisplay() == MaterialDisplay::SolidMask)
        {
            setupMaterialDepthRender(mesh->material);
            d3ddev->SetVertexShader(pUVSimpleMaskVertexShader);
            d3ddev->SetPixelShader(pUVSimpleMaskFragmentShader);
        }
        else
        {
            d3ddev->SetVertexShader(pUVSimpleVertexShader);
            d3ddev->SetPixelShader(pUVSimpleFragmentShader);
        }

        renderMesh(camera, &cameraPosition, mesh->mesh, mesh->model);
    }
}

void DirectX9Renderer::renderMeshShadowDepthData(Camera *camera, Vector3 &cameraPosition, QueuedMeshRenderData *mesh)
{
    if (mesh->bones)
    {
        d3ddev->SetVertexShader(pUVShadowSkinnedVertexShader);
        d3ddev->SetPixelShader(pUVShadowFragmentShader);

        renderMeshSkinned(camera, &cameraPosition, mesh->mesh, mesh->model, mesh->bones);
    }
    else
    {
        if (mesh->material->getDisplay() == MaterialDisplay::Solid)
        {

            d3ddev->SetVertexShader(pUVShadowVertexShader);
            d3ddev->SetPixelShader(pUVShadowFragmentShader);
        }
        else
        {
            setupMaterialDepthRender(mesh->material);
            d3ddev->SetVertexShader(pUVSimpleMaskVertexShader);
            d3ddev->SetPixelShader(pUVSimpleMaskFragmentShader);
        }

        renderMesh(camera, &cameraPosition, mesh->mesh, mesh->model);
    }
}

void DirectX9Renderer::renderShadowBuffers(Vector3 &cameraPosition, Vector3 &cameraFrowardVector)
{
    for (int i = 0; i < data.queueActiveCurrentLight; i++)
    {
        if (data.queueActiveLights[i]->light && data.queueActiveLights[i]->light->isShadowsEnabled())
        {
            if (data.queueActiveLights[i]->light->getType() == LightType::Directional)
                renderShadowBuffersDirectional(cameraPosition, cameraFrowardVector, data.queueActiveLights[i]->light);
        }
    }
}

void DirectX9Renderer::renderShadowBuffersDirectional(Vector3 &cameraPosition, Vector3 &cameraFrowardVector, Light *light)
{
    Vector3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
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

        Vector3 lightShotPosition = cameraPosition + light->getNormal() * cascadeSize * -2.0f + cameraFrowardVector * 0.5f * cascadeSize;
        camera.setupAsOrthographic(cascadeSize, cascadeSize, cascadeSize * -4.0f, cascadeSize * 4.0f);
        cameraEntity.setPosition(lightShotPosition);
        cameraEntity.setRotation(glm::rotation(forward, light->getNormal()));
        camera.updateViewMatrix(cameraEntity.getModelMatrix());

        d3ddev->SetRenderTarget(0, cascade->surface);
        d3ddev->SetDepthStencilSurface(cascade->depthDataSurface);

        d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
        d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        renderQueueLightDepthBuffer(lightShotPosition, &camera);
        if (i == 0)
            light->setShadowViewProjectionMatrix(glm::transpose(*camera.getProjectionMatrix() * *camera.getViewMatrix()));
    }

    d3ddev->SetRenderTarget(0, originalRenderTarget);
    d3ddev->SetDepthStencilSurface(originalDepthStencil);
}