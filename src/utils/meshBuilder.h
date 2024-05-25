// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/mesh.h"

class MeshBuilder
{
public:
    EXPORT Mesh *createCube(float size);
    EXPORT Mesh *createCubeColored(float size, Color color);
    EXPORT Mesh *createCubeColored(float size, Color color[8]);
};
