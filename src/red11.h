// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/math.h"
#include "utils/meshBuilder.h"
#include "utils/deltaCounter.h"
#include "utils/jobQueue.h"
#include "utils/logger.h"
#include "utils/glm/glm.hpp"
#include "utils/glm/gtx/vector_angle.inl"
#include "scene/scene.h"
#include "actor/actor.h"
#include "window/window.h"
#include "renderer/renderer.h"
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "data/camera.h"
#include "data/texture.h"
#include "data/textureFile.h"
#include "data/inputProvider.h"
#include "data/data3DFile.h"
#include "data/debugEntities.h"
#include "data/sound.h"
#include "data/soundFile.h"
#include "audio/audio.h"

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

class Red11
{
protected:
    Red11();

public:
    EXPORT static Scene *createScene();

    EXPORT static Window *createWindow(const char *utf8Name, int width, int height, int flags = 0);

    EXPORT static void openConsole();

    EXPORT static bool isRendererAvailable(RendererType rendererType);
    EXPORT static Renderer *createRenderer(Window *window, RendererType rendererType);

    EXPORT static MeshBuilder *getMeshBuilder();

    EXPORT static InputProvider *getGlobalInputProvider();

    EXPORT static JobQueue *getJobQueue();

    EXPORT static DebugEntities *getDebugEntities();

    EXPORT static Logger *getLogger();

    EXPORT static Audio *getAudio();

protected:
    static MeshBuilder *meshBuilder;
    static InputProvider *globalInputProvider;
    static JobQueue *jobQueue;
    static DebugEntities *debugEntities;
    static Logger *logger;
    static Audio *audio;
};
