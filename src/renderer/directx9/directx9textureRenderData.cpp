// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9textureRenderData.h"

Directx9TextureRenderData::Directx9TextureRenderData(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture)
{
    switch (dataTexture->getType())
    {
    case TextureType::Normal:
        createNormalTexture(d3ddev, dataTexture);
        break;

    case TextureType::ByteMap:
        createByteMapTexture(d3ddev, dataTexture);
        break;

    case TextureType::GpuStencil:
        createStencilTexture(d3ddev, dataTexture);
        break;

    default:
        break;
    }
}

void Directx9TextureRenderData::createNormalTexture(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture)
{
    HRESULT res = d3ddev->CreateTexture(
        dataTexture->getWidth(),
        dataTexture->getHeight(),
        0,
        0,
        D3DFORMAT::D3DFMT_A8R8G8B8,
        D3DPOOL::D3DPOOL_MANAGED,
        &texture,
        nullptr);

    if (res != D3D_OK)
    {
        if (res == D3DERR_INVALIDCALL)
            printf("Invalid D3D call\n");
        if (res == D3DERR_OUTOFVIDEOMEMORY)
            printf("D3D out of memory\n");
        if (res == E_OUTOFMEMORY)
            printf("E out of memory\n");
        return;
    }

    d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

    updIndex = dataTexture->getUpdIndex();

    D3DLOCKED_RECT r;
    texture->LockRect(0, &r, NULL, D3DLOCK_DISCARD);

    unsigned int *pData = (unsigned int *)r.pBits;
    unsigned int *buffer = (unsigned int *)dataTexture->getBufferData();

    int pSize = dataTexture->getWidth() * dataTexture->getHeight();
    for (int i = 0; i < pSize; i++)
    {
        unsigned char *s = (unsigned char *)&buffer[i];
        pData[i] = s[2] + (s[1] << 8) + (s[0] << 16) + (s[3] << 24);
    }

    texture->UnlockRect(0);

    isReadyState = true;
}

void Directx9TextureRenderData::createByteMapTexture(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture)
{
    HRESULT res = d3ddev->CreateTexture(
        dataTexture->getWidth(),
        dataTexture->getHeight(),
        0,
        0,
        D3DFORMAT::D3DFMT_A8,
        D3DPOOL::D3DPOOL_MANAGED,
        &texture,
        nullptr);

    if (res != D3D_OK)
    {
        if (res == D3DERR_INVALIDCALL)
            printf("Invalid D3D call\n");
        if (res == D3DERR_OUTOFVIDEOMEMORY)
            printf("D3D out of memory\n");
        if (res == E_OUTOFMEMORY)
            printf("E out of memory\n");
        return;
    }

    d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

    updIndex = dataTexture->getUpdIndex();

    D3DLOCKED_RECT r;
    texture->LockRect(0, &r, NULL, D3DLOCK_DISCARD);

    memcpy(r.pBits, dataTexture->getBufferData(), dataTexture->getWidth() * dataTexture->getHeight());

    texture->UnlockRect(0);

    isReadyState = true;
}

void Directx9TextureRenderData::createStencilTexture(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture)
{
    HRESULT res = d3ddev->CreateTexture(
        dataTexture->getWidth(),
        dataTexture->getHeight(),
        1,
        D3DUSAGE_DEPTHSTENCIL,
        D3DFMT_D24X8,
        D3DPOOL::D3DPOOL_DEFAULT,
        &depthDataTexture,
        nullptr);

    if (res != D3D_OK)
    {
        if (res == D3DERR_INVALIDCALL)
            printf("Invalid D3D call\n");
        if (res == D3DERR_OUTOFVIDEOMEMORY)
            printf("D3D out of memory\n");
        if (res == E_OUTOFMEMORY)
            printf("E out of memory\n");
        printf("Error creating stencil buffer\n");
        return;
    }

    res = d3ddev->CreateTexture(
        dataTexture->getWidth(),
        dataTexture->getHeight(),
        1,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_R32F,
        D3DPOOL::D3DPOOL_DEFAULT,
        &texture,
        nullptr);

    if (res != D3D_OK)
    {
        if (res == D3DERR_INVALIDCALL)
            printf("Invalid D3D call\n");
        if (res == D3DERR_OUTOFVIDEOMEMORY)
            printf("D3D out of memory\n");
        if (res == E_OUTOFMEMORY)
            printf("E out of memory\n");
        printf("Error creating render target buffer\n");
        return;
    }

    updIndex = dataTexture->getUpdIndex();

    // Get the surface level
    texture->GetSurfaceLevel(0, &surface);

    // Get the stencil level
    depthDataTexture->GetSurfaceLevel(0, &depthDataSurface);

    isReadyState = true;
}