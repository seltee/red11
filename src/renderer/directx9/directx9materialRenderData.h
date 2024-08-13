// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "settings.h"
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <d3d9.h>
#include <vector>
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "directx9utils.h"

class Directx9data;

class Directx9MaterialRenderData
{
public:
    Directx9MaterialRenderData(LPDIRECT3DDEVICE9 d3ddev, Material *material);

    DX9Material *getData();

    void setupForRender(Directx9data *dxData, bool bUseReflectionRadiance);
    void setupForDepth(Directx9data *dxData);

protected:
    void rebuildData();

    LPDIRECT3DDEVICE9 d3ddev;
    Material *material;
    DX9Material data;
    unsigned int lastUpdIndex = 0xffffffff;
};

#endif