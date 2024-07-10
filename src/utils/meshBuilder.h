// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/mesh.h"

class MeshBuilder
{
public:
    EXPORT Mesh *createCube(float size);
    EXPORT Mesh *createSphere(float radius, unsigned int rings = 20, unsigned int segments = 16);
    EXPORT Mesh *createCapsule(Vector3 pa, Vector3 pb, float radius, unsigned int segments = 8);
};
