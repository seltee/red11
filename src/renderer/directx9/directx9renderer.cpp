// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9renderer.h"
#include "window/windows/windowsWindow.h"
#include <string>

#define FVF_COLORED (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define FVF_TEXTURED (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL)

struct VertexNormalColor
{
    float x, y, z;                   // from the D3DFVF_XYZRHW flag
    float normalX, normalY, normalZ; // from the D3DFVF_NORMAL flag
    unsigned int color;              // from the D3DFVF_DIFFUSE flag
};

struct VertexNormalUV
{
    float x, y, z;                   // from the D3DFVF_XYZRHW flag
    float normalX, normalY, normalZ; // from the D3DFVF_NORMAL flag
    float u, v;                      // from the D3DFVF_TEX1 flag
};

DirectX9Renderer::DirectX9Renderer(Window *window) : Renderer(window)
{
    WindowsWindow *winWindow = (WindowsWindow *)window;

    ZeroMemory(meshRenderData, sizeof(MeshRenderData *) * MAX_MESH_COUNT);
    ZeroMemory(materialRenderData, sizeof(MaterialRenderData *) * MAX_MATERIAL_COUNT);
    ZeroMemory(textureRenderData, sizeof(TextureRenderData *) * MAX_TEXTURES_COUNT);
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
        queueCurrentMesh++;
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

    d3ddev->SetTransform(D3DTS_VIEW, (D3DMATRIX *)value_ptr(*camera->getViewMatrix()));
    d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)value_ptr(*camera->getProjectionMatrix())); // set the projection transform

    for (int i = 0; i < queueCurrentMesh; i++)
    {
        renderMesh(queueMeshes[i].mesh, queueMeshes[i].material, queueMeshes[i].model);
    }
}

void DirectX9Renderer::clearQueue()
{
    queueCurrentLight = 0;
    queueCurrentMesh = 0;
}

void DirectX9Renderer::renderMesh(Mesh *mesh, Material *material, Matrix4 *model)
{
    if (!mesh || !material)
        return;

    int meshIndex = mesh->getIndex();
    MeshRenderData *meshData = meshRenderData[meshIndex];
    if (!meshData)
    {
        meshData = createBuffer(mesh);
        meshRenderData[meshIndex] = meshData;
    }

    int materialIndex = material->getIndex();
    MaterialRenderData *materialData = materialRenderData[materialIndex];
    if (!materialData)
    {
        materialData = createSimpleMaterial((MaterialSimple *)material);
        meshRenderData[meshIndex] = meshData;
    }

    auto defTexture = reinterpret_cast<MaterialSimple *>(material)->getDefuseTexture();
    if (defTexture)
    {
        int tIndex = defTexture->getIndex();

        auto textureDXData = textureRenderData[tIndex];
        if (!textureDXData)
        {
            textureDXData = createTexture(defTexture);
            textureRenderData[tIndex] = textureDXData;
        }
        if (textureDXData && textureDXData->texture)
            d3ddev->SetTexture(0, textureDXData->texture);
    }
    else
    {
        d3ddev->SetTexture(0, 0);
    }

    if (mesh->getType() == VertexDataType::PositionUV)
    {
        d3ddev->SetFVF(FVF_TEXTURED);
        d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(VertexNormalUV));
        d3ddev->SetMaterial(&materialData->material); // set the globably-used material to &material
    }
    else
    {
        d3ddev->SetFVF(FVF_COLORED);
        d3ddev->SetStreamSource(0, meshData->vBuffer, 0, sizeof(VertexNormalColor));
        d3ddev->SetMaterial(&materialData->material); // set the globably-used material to &material
    }

    d3ddev->SetIndices(meshData->iBuffer);

    d3ddev->SetTransform(D3DTS_WORLD, (D3DMATRIX *)value_ptr(*model));

    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, meshData->vAmount, 0, meshData->iAmount);
}

void DirectX9Renderer::setAmbientLight(Color &color)
{
    d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB((unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255)));
}

void DirectX9Renderer::setLightningState(bool value)
{
    d3ddev->SetRenderState(D3DRS_LIGHTING, value);
}

void DirectX9Renderer::setZTestState(bool value)
{
    d3ddev->SetRenderState(D3DRS_ZENABLE, value);
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
    for (int i = 0; i < MAX_MATERIAL_COUNT; i++)
    {
        if (materialRenderData[i])
        {
            delete materialRenderData[i];
            materialRenderData[i] = nullptr;
        }
    }
}

MeshRenderData *DirectX9Renderer::createBuffer(Mesh *mesh)
{
    MeshRenderData *data = new MeshRenderData();
    int pAmount = mesh->getPolygonsAmount();
    int vAmount = mesh->getVerticiesAmount();
    auto polies = mesh->getPolygons();
    auto verticies = mesh->getVerticies();

    unsigned int indeciesAmount = pAmount * 3;
    bool useShortForIndicies = indeciesAmount < 0xffff;

    // if mesh data not intended to use UV
    if (mesh->getType() == VertexDataType::PositionColor)
    {
        d3ddev->CreateVertexBuffer(vAmount * sizeof(VertexNormalColor), 0, FVF_COLORED, D3DPOOL_MANAGED, &data->vBuffer, NULL);
        VertexNormalColor *vBufferData;
        data->vBuffer->Lock(0, 0, (void **)&vBufferData, 0); // locks v_buffer, the buffer we made earlier
        for (int i = 0; i < vAmount; i++)
        {
            VertexDataColored v = verticies->vertexPositionColor[i];
            vBufferData[i] = {v.position.x, v.position.y, v.position.z, v.position.x, v.position.y, v.position.z, Color(0.6f, 0.6f, 0.6f).getAsUInt()};
        }
    }

    // if mesh vertex data is just position/normal
    if (mesh->getType() == VertexDataType::PositionUV)
    {
        d3ddev->CreateVertexBuffer(vAmount * sizeof(VertexNormalUV), 0, FVF_TEXTURED, D3DPOOL_MANAGED, &data->vBuffer, NULL);
        VertexNormalUV *vBufferData;
        data->vBuffer->Lock(0, 0, (void **)&vBufferData, 0); // locks v_buffer, the buffer we made earlier
        for (int i = 0; i < vAmount; i++)
        {
            VertexDataUV v = verticies->vertexPositionUV[i];
            vBufferData[i] = {v.position.x, v.position.y, v.position.z, v.normal.x, v.normal.y, v.normal.z, v.uv.x, v.uv.y};
        }
    }

    data->vBuffer->Unlock(); // unlock v_buffer
    data->vAmount = vAmount;

    // Indicies buffer
    D3DFORMAT format = useShortForIndicies ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
    d3ddev->CreateIndexBuffer(useShortForIndicies ? indeciesAmount * sizeof(short) : indeciesAmount * sizeof(int),
                              0,
                              useShortForIndicies ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
                              D3DPOOL_MANAGED,
                              &data->iBuffer,
                              NULL);

    void *iBufferData;
    data->iBuffer->Lock(0, 0, (void **)&iBufferData, 0);

    for (int i = 0; i < pAmount; i++)
    {
        if (format == D3DFMT_INDEX16)
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

    data->iBuffer->Unlock();

    data->iAmount = pAmount;
    data->iFormat = format;

    return data;
}

MaterialRenderData *DirectX9Renderer::createSimpleMaterial(MaterialSimple *material)
{
    MaterialRenderData *data = new MaterialRenderData();
    ZeroMemory(&data->material, sizeof(D3DMATERIAL9));
    Color defuseColor = material->getDefuseColor();
    Color ambientColor = material->getAmbientColor();
    Color emissionColor = material->getEmissionColor();

    data->material.Diffuse = {defuseColor.r, defuseColor.g, defuseColor.b, defuseColor.a};
    data->material.Ambient = {ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a};
    data->material.Emissive = {emissionColor.r, emissionColor.g, emissionColor.b, emissionColor.a};

    return data;
}

TextureRenderData *DirectX9Renderer::createTexture(Texture *texture)
{
    TextureRenderData *data = new TextureRenderData();

    data->texture = nullptr;
    HRESULT res = d3ddev->CreateTexture(texture->getWidth(), texture->getHeight(), 0, 0, D3DFORMAT::D3DFMT_X8R8G8B8, D3DPOOL::D3DPOOL_MANAGED, &data->texture, nullptr);

    if (res != D3D_OK)
    {
        if (res == D3DERR_INVALIDCALL)
            printf("Invalid D3D call\n");
        if (res == D3DERR_OUTOFVIDEOMEMORY)
            printf("D3D out of memory\n");
        if (res == E_OUTOFMEMORY)
            printf("E out of memory\n");
        return nullptr;
    }

    d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

    data->updIndex = texture->getUpdIndex();

    D3DLOCKED_RECT r;
    data->texture->LockRect(0, &r, NULL, D3DLOCK_DISCARD);

    unsigned int *pData = (unsigned int *)r.pBits;
    unsigned int *buffer = (unsigned int *)texture->getBufferData();

    int pSize = texture->getWidth() * texture->getHeight();
    for (int i = 0; i < pSize; i++)
    {
        unsigned char *s = (unsigned char *)&buffer[i];
        pData[i] = s[2] + (s[1] << 8) + (s[0] << 16) + (s[3] << 24);
    }

    data->texture->UnlockRect(0);

    return data;
}
