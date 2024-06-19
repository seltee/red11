// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentMesh.h"

ComponentMesh::ComponentMesh()
{
}

void ComponentMesh::setMesh(Mesh *mesh)
{
    this->mesh = mesh;
}

void ComponentMesh::setMaterial(Material *material)
{
    this->material = material;
}

void ComponentMesh::onRender(Camera *camera, Renderer *renderer)
{
    if (mesh)
    {
        renderer->renderMesh(camera, mesh, material, getModelMatrix());
    }
}

void ComponentMesh::onRenderQueue(Renderer *renderer)
{
    if (mesh)
    {
        renderer->queueMesh(mesh, material, getModelMatrix());
    }
}