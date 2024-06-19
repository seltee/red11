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
    Directx9TextureRenderData(LPDIRECT3DDEVICE9 d3ddev, Texture *texture);

    IDirect3DTexture9 *d3dtexture = nullptr;
    int updIndex;

    bool isReadyState = false;
};

#endif