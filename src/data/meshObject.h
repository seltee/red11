// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "mesh.h"
#include "utils/utils.h"
#include "data/entity.h"

// Mesh with some additional data
// Used when you need to memorize placement the mesh inside some combined model, parenting and other features

class MeshObject : public Entity
{
public:
    EXPORT MeshObject(Mesh *mesh, bool bIsBone = false);
    EXPORT void setParent(MeshObject *parent);
    EXPORT void setEntityParent(Entity *entityParent);

    // generates new mesh
    // mark it temporary if you're going to process it and delete it
    // note: garbage collector may remove temporary meshes on it's own between game cycles
    EXPORT Mesh *getTransformedMesh();

    EXPORT Mesh *getMesh();

    EXPORT MeshObject *clone();

    inline MeshObject *getParent() { return parent; }

    inline std::string *getNamePointer() { return &name; }
    inline void setName(std::string name) { this->name = name; }

    inline bool isBone() { return bIsBone; }

    inline unsigned int getIndex() { return index; }

protected:
    Mesh *mesh = nullptr;
    MeshObject *parent = nullptr;
    std::string name;
    bool bIsBone;

    unsigned int index;
};