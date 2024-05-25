// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/math.h"
#include "utils/meshBuilder.h"
#include "scene/scene.h"
#include "actor/actor.h"
#include "window/window.h"
#include "renderer/renderer.h"
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "data/camera.h"

#ifdef WINDOWS_ONLY
#include <shellscalingapi.h>
#include <windows.h>
#undef DELETE
#endif

#ifdef WINDOWS_ONLY
#define APPMAIN int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define APPMAIN int main(int argc, char *argv[])
#endif

class Shine
{
protected:
    Shine();

public:
    EXPORT static Scene *createScene();

    EXPORT static Window *createWindow(const char *utf8Name, int width, int height, bool bSetFullscreen);

    EXPORT static void openConsole();

    EXPORT static bool isRendererAvailable(RendererType rendererType);
    EXPORT static Renderer *createRenderer(Window *window, RendererType rendererType);

    EXPORT static MeshBuilder *getMeshBuilder();

protected:
    static MeshBuilder *meshBuilder;
};
