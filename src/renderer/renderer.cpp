// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer.h"
#include <math.h>

Renderer::Renderer(Window *window)
{
    this->window = window;
    viewWidth = window->getWidth();
    viewHeight = window->getHeight();

    memset(matrixStore, 0, sizeof(Matrix4) * MAX_MATRICIES);
}

void Renderer::queueMesh(Mesh *mesh, Material *material, Matrix4 &model)
{
    if (lastMatrixStore < MAX_MATRICIES)
    {
        matrixStore[lastMatrixStore] = model;
        this->queueMesh(mesh, material, &matrixStore[lastMatrixStore]);
        lastMatrixStore++;
    }
}