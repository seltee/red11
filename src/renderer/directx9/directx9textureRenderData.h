// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <d3d9.h>
#include <vector>
#include "data/texture.h"
#include "directx9utils.h"

class Directx9TextureRenderData
{
public:
    Directx9TextureRenderData(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture);

    IDirect3DTexture9 *texture = nullptr;
    IDirect3DSurface9 *surface = nullptr;
    IDirect3DTexture9 *depthDataTexture = nullptr;
    IDirect3DSurface9 *depthDataSurface = nullptr;
    int updIndex;

    bool isReadyState = false;

protected:
    void createNormalTexture(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture);
    void createStencilTexture(LPDIRECT3DDEVICE9 d3ddev, Texture *dataTexture);
};

#endif