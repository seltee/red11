// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9textureRenderData.h"

Directx9TextureRenderData::Directx9TextureRenderData(LPDIRECT3DDEVICE9 d3ddev, Texture *texture)
{
    HRESULT res = d3ddev->CreateTexture(texture->getWidth(), texture->getHeight(), 0, 0, D3DFORMAT::D3DFMT_A8R8G8B8, D3DPOOL::D3DPOOL_MANAGED, &d3dtexture, nullptr);

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

    updIndex = texture->getUpdIndex();

    D3DLOCKED_RECT r;
    d3dtexture->LockRect(0, &r, NULL, D3DLOCK_DISCARD);

    unsigned int *pData = (unsigned int *)r.pBits;
    unsigned int *buffer = (unsigned int *)texture->getBufferData();

    int pSize = texture->getWidth() * texture->getHeight();
    for (int i = 0; i < pSize; i++)
    {
        unsigned char *s = (unsigned char *)&buffer[i];
        pData[i] = s[2] + (s[1] << 8) + (s[0] << 16) + (s[3] << 24);
    }

    d3dtexture->UnlockRect(0);
    isReadyState = true;
}