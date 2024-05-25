// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _CRT_SECURE_NO_WARNINGS
#include "shine.h"
#include "renderer/directx9/directx9renderer.h"
#include "window/windows/windowsWindow.h"

MeshBuilder *Shine::meshBuilder = nullptr;

Shine::Shine()
{
}

Scene *Shine::createScene()
{
    return new Scene();
}

Window *Shine::createWindow(const char *utf8Name, int width, int height, bool bSetFullscreen)
{
    return new WindowsWindow(utf8Name, width, height, bSetFullscreen);
}

void Shine::openConsole()
{
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
}

bool Shine::isRendererAvailable(RendererType rendererType)
{
    return true;
}

Renderer *Shine::createRenderer(Window *window, RendererType rendererType)
{
    return new DirectX9Renderer(window);
}

MeshBuilder *Shine::getMeshBuilder()
{
    if (!meshBuilder)
        meshBuilder = new MeshBuilder();
    return meshBuilder;
}