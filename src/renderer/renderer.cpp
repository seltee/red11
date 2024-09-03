// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer.h"
#include <math.h>

std::vector<Renderer *> Renderer::renderers;

Renderer::Renderer(Window *window, AntialiasingMethod antialiasingMethod)
{
    this->window = window;
    this->antialiasingMethod = antialiasingMethod;
    viewWidth = window->getWidth();
    viewHeight = window->getHeight();

    memset(matrixStore, 0, sizeof(Matrix4) * MAX_MATRICIES);
    renderers.push_back(this);
}

Renderer::~Renderer()
{
    auto it = renderers.begin();
    while (it != renderers.end())
    {
        if ((*it) == this)
        {
            renderers.erase(it);
            break;
        }
        it++;
    }
}

void Renderer::queueMesh(Mesh *mesh, Material *material, const Matrix4 &model)
{
    if (lastMatrixStore < MAX_MATRICIES)
    {
        matrixStore[lastMatrixStore] = model;
        this->queueMesh(mesh, material, &matrixStore[lastMatrixStore]);
        lastMatrixStore++;
    }
}

std::vector<AntialiasingMethod> Renderer::getListOfAvailableAntialiasingMethods()
{
    std::vector<AntialiasingMethod> list;
    list.push_back(AntialiasingMethod::None);
    if (isAntialiasingMethodAvailable(AntialiasingMethod::Multisample2))
        list.push_back(AntialiasingMethod::Multisample2);
    if (isAntialiasingMethodAvailable(AntialiasingMethod::Multisample4))
        list.push_back(AntialiasingMethod::Multisample4);
    if (isAntialiasingMethodAvailable(AntialiasingMethod::Multisample8))
        list.push_back(AntialiasingMethod::Multisample8);
    if (isAntialiasingMethodAvailable(AntialiasingMethod::Multisample16))
        list.push_back(AntialiasingMethod::Multisample16);
    return list;
}

void Renderer::removeFromAllTextureByIndex(unsigned int index)
{
    for (auto &item : renderers)
        item->removeTextureByIndex(index);
}

void Renderer::removeFromAllMaterialByIndex(unsigned int index)
{
    for (auto &item : renderers)
        item->removeMaterialByIndex(index);
}

void Renderer::removeFromAllMeshByIndex(unsigned int index)
{
    for (auto &item : renderers)
        item->removeMeshByIndex(index);
}