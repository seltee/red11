// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "settings.h"
#include "utils/utils.h"
#include "data/shader.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <vector>
#include <string>

class DirectX9Shader : public Shader
{
public:
    DirectX9Shader(const std::string &name, LPDIRECT3DDEVICE9 d3ddev, const DWORD *vsoCode, const DWORD *psoCode);
    RendererType getType() override final;
    void use() override final;

    inline bool isGood() { return bIsGood; }

protected:
    std::string name;
    LPDIRECT3DDEVICE9 d3ddev;

    IDirect3DVertexShader9 *pVertexShader = nullptr;
    IDirect3DPixelShader9 *pFragmentShader = nullptr;

    bool bIsGood = false;
};

#endif