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
    ZeroMemory(materialRenderData, sizeof(Directx9MaterialRenderData *) * MAX_MATERIALS_COUNT);
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
    if (queueCurrentMesh < MAX_QUEUE_MESH_COUNT && mesh)
    {
        queueMeshes[queueCurrentMesh].mesh = mesh;
        queueMeshes[queueCurrentMesh].material = material ? material : defaultMaterial;
        queueMeshes[queueCurrentMesh].model = model;
        queueMeshes[queueCurrentMesh].bones = nullptr;
        queueCurrentMesh++;
    }
}

void DirectX9Renderer::queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones)
{
    if (queueCurrentMesh < MAX_QUEUE_MESH_COUNT && mesh)
    {
        queueMeshes[queueCurrentMesh].mesh = mesh;
        queueMeshes[queueCurrentMesh].material = material ? material : defaultMaterial;
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
    renderQueueDepthBuffer(camera);
    renderQueueDepthEqual(camera);
}

void DirectX9Renderer::clearQueue()
{
    queueCurrentLight = 0;
    queueCurrentMesh = 0;
    queueCurrentLine = 0;
}

void DirectX9Renderer::renderMesh(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model)
{
    if (!mesh)
        return;
    Directx9MeshRenderData *meshData = getMeshRenderData(mesh);

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
    Directx9MeshRenderData *meshData = getMeshRenderData(mesh);

    // set bone matrices, base register is 52, so, 51 bone is availalbe
    for (auto &bone : *bones)
    {
        Deform *deform = mesh->getDeformByName(*bone.name);
        if (deform)
        {
            int reg = 52 + deform->index * 4;
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

    Vector3 camPosition = Vector3(Vector4(0.0f, 0.0f, 0.0f, 1.0f) * *camera->getWorldMatrix());
    renderMesh(camera, &camPosition, cubeMesh, entity.getModelMatrix());
}

void DirectX9Renderer::setAmbientLight(Color &ambientColor)
{
    this->ambientColor = ambientColor;
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
                      D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
                      &d3dpp,
                      &d3ddev);

    d3ddev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

    cubeMesh = Red11::getMeshBuilder()->createCube(1.0f);
    lineMaterial = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.2, 1.0, 0.2));

    if (d3ddev->CreateVertexShader((const DWORD *)UVSimpleVertexShader_vso, &pUVSimpleVertexShader) != D3D_OK)
        printf("Shader compilation failed\n");
    if (d3ddev->CreatePixelShader((const DWORD *)UVSimpleFragmentShader_pso, &pUVSimpleFragmentShader) != D3D_OK)
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

    Vector3 camPosition = Vector3(*camera->getWorldMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    for (int i = 0; i < queueCurrentMesh; i++)
    {
        if (queueMeshes[i].bones)
        {
            d3ddev->SetVertexShader(pUVSkinnedVertexShader);
            d3ddev->SetPixelShader(pUVSkinnedFragmentShader);

            renderMeshSkinned(camera, &camPosition, queueMeshes[i].mesh, queueMeshes[i].model, queueMeshes[i].bones);
        }
        else
        {
            d3ddev->SetVertexShader(pUVSimpleVertexShader);
            d3ddev->SetPixelShader(pUVSimpleFragmentShader);

            renderMesh(camera, &camPosition, queueMeshes[i].mesh, queueMeshes[i].model);
        }
    }
}

void DirectX9Renderer::renderQueueDepthEqual(Camera *camera)
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
    d3ddev->SetPixelShaderConstantF(14, ambientColor.getAsFloatArray(), 1);

    // Camera position is shared among all render targets
    Vector3 camPosition = Vector3(*camera->getWorldMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    d3ddev->SetPixelShaderConstantF(17, (const float *)value_ptr(camPosition), 1);

    for (int i = 0; i < queueCurrentMesh; i++)
    {
        setupLights(*queueMeshes[i].model * Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.1f);
        setupMaterial(queueMeshes[i].material);

        if (queueMeshes[i].bones)
        {
            if (queueMeshes[i].material->isUsingNormalMap())
            {
                d3ddev->SetVertexShader(pUVSkinnedNormalVertexShader);
                d3ddev->SetPixelShader(pUVSkinnedNormalFragmentShader);
            }
            else
            {
                d3ddev->SetVertexShader(pUVSkinnedVertexShader);
                d3ddev->SetPixelShader(pUVSkinnedFragmentShader);
            }

            renderMeshSkinned(camera, &camPosition, queueMeshes[i].mesh, queueMeshes[i].model, queueMeshes[i].bones);
        }
        else
        {
            if (queueMeshes[i].material->isUsingNormalMap())
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
            renderMesh(camera, &camPosition, queueMeshes[i].mesh, queueMeshes[i].model);
        }
    }

    if (queueCurrentLine > 0)
    {
        d3ddev->SetRenderState(D3DRS_ZENABLE, false);
        setupMaterial(lineMaterial);
        for (int i = 0; i < queueCurrentLine; i++)
        {
            // Emission Color
            Vector4 color = Vector4(queueLines[i].color.r, queueLines[i].color.g, queueLines[i].color.b, 1.0f);
            d3ddev->SetPixelShaderConstantF(16, (const float *)value_ptr(color), 1);

            d3ddev->SetVertexShader(pUVVertexShader);
            d3ddev->SetPixelShader(pUVFragmentShader);

            renderLine(camera, queueLines[i].vFrom, queueLines[i].vTo);
        }
    }
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

Directx9MaterialRenderData *DirectX9Renderer::getMaterialRenderData(Material *material)
{
    if (!material)
        return nullptr;

    int materialIndex = material->getIndex();
    Directx9MaterialRenderData *materialData = materialRenderData[materialIndex];
    if (!materialData)
    {
        materialData = new Directx9MaterialRenderData(material);
        materialRenderData[materialIndex] = materialData;
    }
    return materialData;
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

void DirectX9Renderer::setupLights(Vector3 objectPosition, float objectRadius)
{
    // registers for lights:
    // 16 - 48 - light data
    // type, typeData, typeData, typeData
    // positionv3, xxx
    // normalv3, power
    // colorv3, radius

    std::vector<Light *> affectingLights;
    for (int lightNum = 0; lightNum < queueCurrentLight; lightNum++)
    {
        Light *lightData = queueLights[lightNum].light;
        if (lightData->isAffecting(objectPosition, objectRadius))
            affectingLights.push_back(lightData);
    }

    affectingLights.resize(MAX_LIGHTS_PER_MESH_COUNT);

    int baseReg = 20;
    DX9LightShaderStruct dxLight;
    memset(&dxLight, 0, sizeof(DX9LightShaderStruct));

    for (auto &light : affectingLights)
    {
        if (!light)
        {
            dxLight.type = 0.0f;
            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 1);
        }
        else if (light->getType() == LightType::Directional)
        {
            Color lightColor = light->getColor();
            Vector3 lightDirection = light->getNormal();

            dxLight.type = 1.0f;
            dxLight.data0 = 1.0f;
            dxLight.data1 = 1.0f;
            dxLight.normal[0] = lightDirection.x;
            dxLight.normal[1] = lightDirection.y;
            dxLight.normal[2] = lightDirection.z;
            dxLight.color[0] = lightColor.r;
            dxLight.color[1] = lightColor.g;
            dxLight.color[2] = lightColor.b;

            d3ddev->SetPixelShaderConstantF(baseReg, (const float *)&dxLight, 4);
        }
        baseReg += 4;
    }

    /*
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
        */
}

void DirectX9Renderer::setupMaterial(Material *material)
{
    MaterialSimple *materialSimple = reinterpret_cast<MaterialSimple *>(material);

    Directx9TextureRenderData *albedoTextureData = materialSimple->getAlbedoTexture() ? getTextureRenderData(materialSimple->getAlbedoTexture()) : nullptr;
    Directx9TextureRenderData *emissionTextureData = materialSimple->getEmissionTexture() ? getTextureRenderData(materialSimple->getEmissionTexture()) : nullptr;
    Directx9TextureRenderData *normalTextureData = materialSimple->getNormalTexture() ? getTextureRenderData(materialSimple->getNormalTexture()) : nullptr;
    Directx9TextureRenderData *metallicTextureData = materialSimple->getMetallicTexture() ? getTextureRenderData(materialSimple->getMetallicTexture()) : nullptr;
    Directx9TextureRenderData *roughnessTextureData = materialSimple->getRoughnessTexture() ? getTextureRenderData(materialSimple->getRoughnessTexture()) : nullptr;
    Directx9TextureRenderData *aoTextureData = materialSimple->getAOTexture() ? getTextureRenderData(materialSimple->getAOTexture()) : nullptr;

    // === Setup material data ===
    Directx9MaterialRenderData *materialRenderData = getMaterialRenderData(material);
    d3ddev->SetPixelShaderConstantF(12, (const float *)materialRenderData->getData(), 2);

    // === Setup colors ===
    // Albedo Color
    d3ddev->SetPixelShaderConstantF(15, materialSimple->getAlbedoColor().getAsFloatArray(), 1);

    // Emission Color
    d3ddev->SetPixelShaderConstantF(16, materialSimple->getEmissionColor().getAsFloatArray(), 1);

    // === Setup textures ===
    if (albedoTextureData)
        d3ddev->SetTexture(0, albedoTextureData->d3dtexture);
    if (emissionTextureData)
        d3ddev->SetTexture(1, emissionTextureData->d3dtexture);
    if (normalTextureData)
        d3ddev->SetTexture(2, normalTextureData->d3dtexture);
    if (metallicTextureData)
        d3ddev->SetTexture(3, metallicTextureData->d3dtexture);
    if (roughnessTextureData)
        d3ddev->SetTexture(4, roughnessTextureData->d3dtexture);
    if (aoTextureData)
        d3ddev->SetTexture(5, aoTextureData->d3dtexture);
}