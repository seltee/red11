// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9renderer.h"
#include "red11.h"
#include "window/windows/windowsWindow.h"
#include <string>

DirectX9Renderer::DirectX9Renderer(Window *window) : Renderer(window)
{
    WindowsWindow *winWindow = (WindowsWindow *)window;

    ZeroMemory(meshRenderData, sizeof(Directx9MeshRenderData *) * MAX_MESH_COUNT);
    ZeroMemory(textureRenderData, sizeof(Directx9TextureRenderData *) * MAX_TEXTURES_COUNT);
    ZeroMemory(queueMeshes, sizeof(QueuedMeshRenderData) * MAX_QUEUE_MESH_COUNT);
    ZeroMemory(queueLights, sizeof(QueuedLightRenderData) * MAX_QUEUE_LIGHTS_COUNT);

    initD3D(winWindow->getHwnd(), winWindow->isFullscreen(), window->getWidth(), window->getHeight());
}

RendererType DirectX9Renderer::getType()
{
    return RendererType::DirectX9;
}

void DirectX9Renderer::startRendering()
{
    d3ddev->BeginScene();
}

void DirectX9Renderer::clearBuffer(Color color)
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(color.getRedAsUChar(), color.getGreenAsUChar(), color.getBlueAsUChar()), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
}

void DirectX9Renderer::queueMesh(Mesh *mesh, Material *material, Matrix4 *model)
{
    if (queueCurrentMesh < MAX_QUEUE_MESH_COUNT && mesh && material)
    {
        queueMeshes[queueCurrentMesh].mesh = mesh;
        queueMeshes[queueCurrentMesh].material = material;
        queueMeshes[queueCurrentMesh].model = model;
        queueMeshes[queueCurrentMesh].bones = nullptr;
        queueCurrentMesh++;
    }
}

void DirectX9Renderer::queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones)
{
    if (queueCurrentMesh < MAX_QUEUE_MESH_COUNT && mesh && material)
    {
        queueMeshes[queueCurrentMesh].mesh = mesh;
        queueMeshes[queueCurrentMesh].material = material;
        queueMeshes[queueCurrentMesh].model = model;
        queueMeshes[queueCurrentMesh].bones = bones;
        queueCurrentMesh++;
    }
}

void DirectX9Renderer::queueLine(Vector3 vFrom, Vector3 vTo, Color color)
{
    if (queueCurrentLine < MAX_QUEUE_LINES)
    {
        queueLines[queueCurrentLine].vFrom = vFrom;
        queueLines[queueCurrentLine].vTo = vTo;
        queueLines[queueCurrentLine].color = color;
        queueCurrentLine++;
    }
}

void DirectX9Renderer::queueLight(Light *light)
{
    if (queueCurrentLight < MAX_QUEUE_LIGHTS_COUNT)
    {
        queueLights[queueCurrentLight].light = light;
        queueCurrentLight++;
    }
}

void DirectX9Renderer::renderQueue(Camera *camera)
{
    // DirectX 9 supports up to 8 lights when using pipeline
    useNearestLightsOnly = MAX_LIGHTS_PER_MESH_COUNT < queueCurrentLight;
    if (!useNearestLightsOnly)
    {
        int lightNum = 0;
        for (; lightNum < queueCurrentLight; lightNum++)
        {
            Light *lightData = queueLights[lightNum].light;
            D3DLIGHT9 light;
            ZeroMemory(&light, sizeof(light));

            if (lightData->getType() == LightType::Directional)
            {
                Color color = lightData->getColor();
                Vector3 direction = lightData->getNormal();

                light.Type = D3DLIGHT_DIRECTIONAL;
                light.Diffuse = {color.r, color.g, color.b, color.a};
                light.Direction = {direction.x, direction.y, direction.z};
            }

            if (lightData->getType() == LightType::Omni)
            {
                Color color = lightData->getColor();
                Vector3 position = lightData->getPosition();
                Attenuation attenuation = lightData->getAttenuation();

                light.Type = D3DLIGHT_POINT;
                light.Diffuse = {color.r, color.g, color.b, color.a};
                light.Position = {position.x, position.y, position.z};
                light.Range = lightData->getRange();
                light.Attenuation0 = attenuation.att0;
                light.Attenuation1 = attenuation.att1;
                light.Attenuation2 = attenuation.att2;
            }

            if (lightData->getType() == LightType::Spot)
            {
                Color color = lightData->getColor();
                Vector3 position = lightData->getPosition();
                Vector3 direction = lightData->getNormal();
                Attenuation attenuation = lightData->getAttenuation();

                light.Type = D3DLIGHT_SPOT;
                light.Diffuse = {color.r, color.g, color.b, color.a};
                light.Position = {position.x, position.y, position.z};
                light.Direction = {direction.x, direction.y, direction.z};
                light.Range = lightData->getRange();
                light.Attenuation0 = attenuation.att0;
                light.Attenuation1 = attenuation.att1;
                light.Attenuation2 = attenuation.att2;
                light.Phi = lightData->getRadius();
                light.Theta = lightData->getInnerRadius();
                light.Falloff = lightData->getFalloff();
            }

            d3ddev->SetLight(lightNum, &light);
            d3ddev->LightEnable(lightNum, true);
        }

        // disable old lights
        for (; lightNum < MAX_QUEUE_LIGHTS_COUNT; lightNum++)
        {
            if (queueLights[lightNum].enabled)
            {
                d3ddev->LightEnable(lightNum, false);
                queueLights[lightNum].enabled = false;
            }
            else
                break;
        }
    }

    // d3ddev->SetTransform(D3DTS_VIEW, (D3DMATRIX *)value_ptr(*camera->getViewMatrix()));
    // d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)value_ptr(*camera->getProjectionMatrix())); // set the projection transform

    setDepthBufferState(true);
    setLightningState(true);

    for (int i = 0; i < queueCurrentMesh; i++)
    {
        if (queueMeshes[i].bones)
            renderMeshSkinned(camera, queueMeshes[i].mesh, queueMeshes[i].material, queueMeshes[i].model, queueMeshes[i].bones);
        else
            renderMesh(camera, queueMeshes[i].mesh, queueMeshes[i].material, queueMeshes[i].model);
    }

    setDepthBufferState(false);
    setLightningState(false);
    for (int i = 0; i < queueCurrentLine; i++)
    {
        renderLine(camera, queueLines[i].vFrom, queueLines[i].vTo, queueLines[i].color);
    }
}

void DirectX9Renderer::clearQueue()
{
    queueCurrentLight = 0;
    queueCurrentMesh = 0;
    queueCurrentLine = 0;
}

void DirectX9Renderer::renderMesh(Camera *camera, Mesh *mesh, Material *material, Matrix4 *model)
{
    if (!mesh || !material)
        return;

    Directx9MeshRenderData *meshData = getMeshRenderData(mesh);
    MaterialSimple *materialSimple = reinterpret_cast<MaterialSimple *>(material);
    Directx9TextureRenderData *defTexture = getTextureRenderData(materialSimple->getDefuseTexture());

    if (mesh->getType() == VertexDataType::PositionUV)
    {
        d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUV));
        d3ddev->SetVertexDeclaration(pVertexDeclNormalUV);
        d3ddev->SetVertexShader(pVertexUVShader);
        d3ddev->SetPixelShader(pFragmentUVShader);

        // Is defuse texture set
        float textureIsSet = defTexture ? true : false;
        d3ddev->SetPixelShaderConstantF(7, &textureIsSet, 1);

        // Defuse Color
        d3ddev->SetPixelShaderConstantF(4, materialSimple->getDefuseColor().getAsFloatArray(), 1);

        // Ambient Color
        d3ddev->SetPixelShaderConstantF(5, ambientColor.getAsFloatArray(), 1);

        // Emission Color
        d3ddev->SetPixelShaderConstantF(6, materialSimple->getEmissionColor().getAsFloatArray(), 1);

        d3ddev->SetTexture(0, defTexture ? defTexture->d3dtexture : 0);
    }
    else if (mesh->getType() == VertexDataType::PositionColor)
    {
        d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalColor));
        d3ddev->SetVertexDeclaration(pVertexDeclNormalColor);
        d3ddev->SetVertexShader(pVertexColorShader);
        d3ddev->SetPixelShader(pFragmentColorShader);
    }

    d3ddev->SetIndices(meshData->iBuffer);

    Matrix4 worldViewProjection = *camera->getProjectionMatrix() * *camera->getViewMatrix() * *model;
    worldViewProjection = glm::transpose(worldViewProjection);
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(worldViewProjection), 4);

    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::renderMeshSkinned(Camera *camera, Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones)
{
    if (!mesh || !material)
        return;

    Directx9MeshRenderData *meshData = getMeshRenderData(mesh);
    MaterialSimple *materialSimple = reinterpret_cast<MaterialSimple *>(material);
    Directx9TextureRenderData *defTexture = getTextureRenderData(materialSimple->getDefuseTexture());

    d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(DX9VertexNormalUVSkinned));
    d3ddev->SetVertexDeclaration(pVertexDeclNormalUVSkinned);
    d3ddev->SetVertexShader(pVertexUVSkinnedShader);
    d3ddev->SetPixelShader(pFragmentUVSkinnedShader);

    // set bone matrices, base register is 48
    for (auto &bone : *bones)
    {
        Deform *deform = mesh->getDeformByName(*bone.name);
        if (deform)
        {
            int reg = 48 + deform->index * 4;
            Matrix4 boneMatrix = *bone.model * deform->getInvBindMatrix();
            boneMatrix = glm::transpose(boneMatrix);
            d3ddev->SetVertexShaderConstantF(reg, (const float *)value_ptr(boneMatrix), 4);
        }
    }

    // Shader multiplies model matrix internally, so view projection needs to be provcided
    Matrix4 viewProjection = *camera->getProjectionMatrix() * *camera->getViewMatrix();
    viewProjection = glm::transpose(viewProjection);
    d3ddev->SetVertexShaderConstantF(0, (const float *)value_ptr(viewProjection), 4);

    // Defuse Color
    d3ddev->SetPixelShaderConstantF(4, materialSimple->getDefuseColor().getAsFloatArray(), 1);

    // Ambient Color
    d3ddev->SetPixelShaderConstantF(5, ambientColor.getAsFloatArray(), 1);

    // Emission Color
    d3ddev->SetPixelShaderConstantF(6, materialSimple->getEmissionColor().getAsFloatArray(), 1);

    // Defuse Texture
    float textureIsSet = defTexture ? true : false;
    d3ddev->SetPixelShaderConstantF(7, &textureIsSet, 1);
    d3ddev->SetTexture(0, defTexture ? defTexture->d3dtexture : 0);

    d3ddev->SetIndices(meshData->iBuffer);

    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::renderLine(Camera *camera, Vector3 vFrom, Vector3 vTo, Color color)
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

    renderMesh(camera, cubeMesh, lineMaterial, entity.getModelMatrix());
}

void DirectX9Renderer::setAmbientLight(Color &ambientColor)
{
    this->ambientColor = ambientColor;
}

void DirectX9Renderer::setLightningState(bool value)
{
    d3ddev->SetRenderState(D3DRS_LIGHTING, value);
}

void DirectX9Renderer::setDepthBufferState(bool state)
{
    d3ddev->SetRenderState(D3DRS_ZENABLE, state);
}

void DirectX9Renderer::endRendering()
{
    d3ddev->EndScene();
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
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);

    d3ddev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

    cubeMesh = Red11::getMeshBuilder()->createCube(1.0f);
    lineMaterial = new MaterialSimple(Color(0.2, 1.0, 0.2));

    if (d3ddev->CreateVertexShader((const DWORD *)vertexColorShader_vso, &pVertexColorShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)fragmentColorShader_pso, &pFragmentColorShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)vertexUVShader_vso, &pVertexUVShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)fragmentUVShader_pso, &pFragmentUVShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreateVertexShader((const DWORD *)vertexUVSkinnedShader_vso, &pVertexUVSkinnedShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)fragmentUVSkinnedShader_pso, &pFragmentUVSkinnedShader) != D3D_OK)
        printf("Shader compilation failed\n");

    D3DVERTEXELEMENT9 VertexElementsNormalColor[] = {
        {0, offsetof(DX9VertexNormalColor, x), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(DX9VertexNormalColor, normalX), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(DX9VertexNormalColor, color), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    D3DVERTEXELEMENT9 VertexElementsNormalUV[] = {
        {0, offsetof(DX9VertexNormalUV, x), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(DX9VertexNormalUV, normalX), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(DX9VertexNormalUV, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    D3DVERTEXELEMENT9 VertexElementsNormalUVSkinned[] = {
        {0, offsetof(DX9VertexNormalUVSkinned, x), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, normalX), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, boneIndices), D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        {0, offsetof(DX9VertexNormalUVSkinned, boneWeights), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        D3DDECL_END()};

    d3ddev->CreateVertexDeclaration(VertexElementsNormalColor, &pVertexDeclNormalColor);
    d3ddev->CreateVertexDeclaration(VertexElementsNormalUV, &pVertexDeclNormalUV);
    d3ddev->CreateVertexDeclaration(VertexElementsNormalUVSkinned, &pVertexDeclNormalUVSkinned);
}

void DirectX9Renderer::renderFrame(void)
{
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
    for (int i = 0; i < MAX_MESH_COUNT; i++)
    {
        if (meshRenderData[i])
        {
            delete meshRenderData[i];
            meshRenderData[i] = nullptr;
        }
    }
}

Directx9MeshRenderData *DirectX9Renderer::getMeshRenderData(Mesh *mesh)
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

Directx9TextureRenderData *DirectX9Renderer::getTextureRenderData(Texture *texture)
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