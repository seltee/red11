// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _CRT_SECURE_NO_WARNINGS
#include "red11.h"
#include "renderer/directx9/directx9renderer.h"
#include "window/windows/windowsWindow.h"

MeshBuilder *Red11::meshBuilder = nullptr;
InputProvider *Red11::globalInputProvider = nullptr;

Red11::Red11()
{
}

Scene *Red11::createScene()
{
    return new Scene();
}

Window *Red11::createWindow(const char *utf8Name, int width, int height, bool bSetFullscreen)
{
    return new WindowsWindow(utf8Name, width, height, bSetFullscreen);
}

void Red11::openConsole()
{
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
}

bool Red11::isRendererAvailable(RendererType rendererType)
{
    return true;
}

Renderer *Red11::createRenderer(Window *window, RendererType rendererType)
{
    return new DirectX9Renderer(window);
}

MeshBuilder *Red11::getMeshBuilder()
{
    if (!meshBuilder)
        meshBuilder = new MeshBuilder();
    return meshBuilder;
}

InputProvider *Red11::getGlobalInputProvider()
{
    if (!globalInputProvider)
        globalInputProvider = new InputProvider();
    return globalInputProvider;
}