// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "window/window.h"
#include "utils/primitives.h"
#include "utils/defines.h"
#include "data/mesh.h"
#include "data/boneTransform.h"
#include "data/material/material.h"
#include "data/material/materialSimple.h"
#include "data/light.h"
#include "data/camera.h"
#include "data/texture.h"
#include <vector>

#define MAX_MATRICIES (256 * 1024)

class Renderer
{
public:
    Renderer(Window *window);
    virtual ~Renderer();

    virtual RendererType getType() = 0;

    virtual void prepareToRender(Texture *ambientTexture = nullptr, Texture *radianceTexture = nullptr) = 0;
    virtual void clearBuffer(Color color) = 0;
    virtual void renderCubeMap(Camera *camera, Entity *entity,Texture *hdr) = 0;

    // This function queues everything by pointers without any copies. Be carefull not to pass a temporary object
    virtual void queueMesh(Mesh *mesh, Material *material, Matrix4 *model) = 0;

    // This function uses small internal store to convert temporary matrix into a permanent
    // This means you can use temporary calculated matrix with this function but mesh and material should be percictent unto render is complete
    // This is made to avoid saving the entire matrix in the queue for most of the objects but sometimes it's nessasary and functionality provided by this function
    void queueMesh(Mesh *mesh, Material *material, Matrix4 &model);

    virtual void queueMeshSkinned(Mesh *mesh, Material *material, Matrix4 *model, std::vector<BoneTransform> *bones) = 0;
    virtual void queueLine(Vector3 vFrom, Vector3 vTo, Color color) = 0;
    virtual void queueLight(Light *light) = 0;
    virtual void renderQueue(Camera *camera) = 0;
    virtual void clearQueue() = 0;
    virtual void renderMesh(Camera *camera, Mesh *mesh, Matrix4 *model) = 0;
    virtual void renderMeshSkinned(Camera *camera, Mesh *mesh, Matrix4 *model, std::vector<BoneTransform> *bones) = 0;
    virtual void setAmbientLight(const Color &ambientColor) = 0;
    virtual void present() = 0;
    virtual void setupSpriteRendering(Matrix4 &mView, Matrix4 &mProjection) = 0;
    virtual void renderSpriteRect(Matrix4 *mModel, const Color color) = 0;
    virtual void renderSpriteMask(Matrix4 *mModel, Texture *texture, const Color color) = 0;
    virtual void renderSpriteImage(Matrix4 *mModel, Texture *texture) = 0;

    inline int getViewWidth() { return viewWidth; }
    inline int getViewHeight() { return viewHeight; }

    virtual void removeTextureByIndex(unsigned int index) = 0;
    virtual void removeMaterialByIndex(unsigned int index) = 0;
    virtual void removeMeshByIndex(unsigned int index) = 0;

    static void removeFromAllTextureByIndex(unsigned int index);
    static void removeFromAllMaterialByIndex(unsigned int index);
    static void removeFromAllMeshByIndex(unsigned int index);

protected:
    int viewWidth = 0, viewHeight = 0;
    Window *window = nullptr;
    Matrix4 matrixStore[MAX_MATRICIES];
    int lastMatrixStore = 0;

    static std::vector<Renderer *> renderers;
};