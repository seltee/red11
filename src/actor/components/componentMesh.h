// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "component.h"
#include "data/mesh.h"
#include "data/material/material.h"
#include "utils/utils.h"

class ComponentMesh : public Component
{
public:
    EXPORT ComponentMesh();

    EXPORT void onRender(Renderer *renderer) override final;
    EXPORT void onRenderQueue(Renderer *renderer) override final;

    EXPORT void setMesh(Mesh *mesh);
    inline Mesh *getMesh() { return mesh; }
    EXPORT void setMaterial(Material *material);
    inline Material *getMaterial() { return material; }

protected:
    Mesh *mesh = nullptr;
    Material *material = nullptr;
};