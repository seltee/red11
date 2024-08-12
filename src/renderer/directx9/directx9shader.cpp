// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9shader.h"
#include "red11.h"

DirectX9Shader::DirectX9Shader(std::string name, LPDIRECT3DDEVICE9 d3ddev, const DWORD *vsoCode, const DWORD *psoCode)
{
    this->name = name;
    this->d3ddev = d3ddev;

    if (d3ddev->CreateVertexShader(vsoCode, &pVertexShader) != D3D_OK)
    {
        Red11::getLogger()->logFileAndConsole("Vertex \"%s\" shader compilation failed", name.c_str());
        return;
    }
    if (d3ddev->CreatePixelShader(psoCode, &pFragmentShader) != D3D_OK)
    {
        Red11::getLogger()->logFileAndConsole("Fragment \"%s\" shader compilation failed\n", name.c_str());
        return;
    }

    bIsGood = true;
}

RendererType DirectX9Shader::getType()
{
    return RendererType::DirectX9;
}

void DirectX9Shader::use()
{
    if (bIsGood)
    {
        d3ddev->SetVertexShader(pVertexShader);
        d3ddev->SetPixelShader(pFragmentShader);
    }
}
