// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "window/window.h"
#include "utils/primitives.h"
#include "data/mesh.h"
#include "data/boneTransform.h"
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "data/light.h"
#include "data/camera.h"
#include "data/texture.h"
#include <vector>

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

    virtual void clearBuffer(Color color) = 0;
    virtual void queueMesh(Mesh *mesh, Material *material, Matrix4 *model) = 0;
    virtual void queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones) = 0;
    virtual void queueLine(Vector3 vFrom, Vector3 vTo, Color color) = 0;
    virtual void queueLight(Light *light) = 0;
    virtual void renderQueue(Camera *camera) = 0;
    virtual void clearQueue() = 0;
    virtual void renderMesh(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model) = 0;
    virtual void renderMeshSkinned(Camera *camera, Vector3 *cameraPosition, Mesh *mesh, Matrix4 *model, std::vector<BoneTransform> *bones) = 0;
    virtual void setAmbientLight(Color &ambientColor) = 0;
    virtual void present() = 0;

protected:
    int viewWidth = 0, viewHeight = 0;
};