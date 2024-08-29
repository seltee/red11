// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _CRT_SECURE_NO_WARNINGS
#include "network/windows/windowsServer.h"
#include "network/windows/windowsClient.h"
#include "renderer/directx9/directx9renderer.h"
#include "window/windows/windowsWindow.h"
#include "audio/windows/audioWindows.h"
#include "red11.h"

MeshBuilder *Red11::meshBuilder = nullptr;
InputProvider *Red11::globalInputProvider = nullptr;
JobQueue *Red11::jobQueue = nullptr;
DebugEntities *Red11::debugEntities = nullptr;
Logger *Red11::logger = nullptr;
Audio *Red11::audio = nullptr;
ResourceManager *Red11::resourceManager = nullptr;

Red11::Red11()
{
}

Scene *Red11::createScene()
{
    return new Scene(getDebugEntities());
}

Window *Red11::createWindow(std::string name, int width, int height, int flags)
{
    return new WindowsWindow(name.c_str(), width, height, flags);
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

Renderer *Red11::createRenderer(Window *window, RendererType rendererType, bool bVSync)
{
    return new DirectX9Renderer(window, bVSync);
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

JobQueue *Red11::getJobQueue()
{
    if (!jobQueue)
        jobQueue = new JobQueue();
    return jobQueue;
}

DebugEntities *Red11::getDebugEntities()
{
    if (!debugEntities)
        debugEntities = new DebugEntities();
    return debugEntities;
}

Logger *Red11::getLogger()
{
    if (!logger)
        logger = new Logger("log.txt");
    return logger;
}

Audio *Red11::getAudio()
{
    if (!audio)
        audio = new AudioWindows();
    return audio;
}

ResourceManager *Red11::getResourceManager()
{
    if (!resourceManager)
        resourceManager = new ResourceManager();
    return resourceManager;
}

Server *Red11::createServer(NetworkApi &networkApi, int port, FuncMessageReceiverCreator funcCreateMessageReceiver)
{
    return new WindowsServer(networkApi, port, funcCreateMessageReceiver);
}

Client *Red11::createClient(NetworkApi &networkApi, MessageReceiver &messageReceiver, const std::string &address, int port)
{
    return new WindowsClient(networkApi, messageReceiver, address, port);
}