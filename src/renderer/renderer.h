// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "window/window.h"
#include "utils/primitives.h"
#include "data/mesh.h"
#include "data/material/material.h"
#include "data/light.h"
#include "data/camera.h"

enum class RendererType
{
    DirectX9,
    OpenGL2,
    DirectX11,
    OpenGL4
};

class Renderer
{
public:
    Renderer(Window *window);

    virtual RendererType getType() = 0;

    virtual void startRendering() = 0;
    virtual void clearBuffer(Color color) = 0;
    virtual void queueMesh(Mesh *mesh, Material *material, Matrix4 *model) = 0;
    virtual void queueLight(Light *light) = 0;
    virtual void renderQueue(Camera *camera) = 0;
    virtual void clearQueue() = 0;
    virtual void renderMesh(Mesh *mesh, Material *material, Matrix4 *model) = 0;
    virtual void setAmbientLight(Color &color) = 0;
    virtual void setLightningState(bool value) = 0;
    virtual void setZTestState(bool value) = 0;
    virtual void endRendering() = 0;
    virtual void present() = 0;

protected:
    int viewWidth = 0, viewHeight = 0;
};