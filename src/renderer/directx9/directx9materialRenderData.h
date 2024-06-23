// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <d3d9.h>
#include <vector>
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "directx9utils.h"

class Directx9MaterialRenderData
{
public:
    Directx9MaterialRenderData(Material *material);

    DX9Material *getData();

protected:
    void rebuildData();

    Material *material;
    DX9Material data;
    unsigned int lastUpdIndex = 0xffffffff;
};

#endif