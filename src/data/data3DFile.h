// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/mesh.h"
#include "data/animation/animation.h"
#include "data/meshObject.h"
#include "data/spline.h"
#include <vector>
#include <string>

// this class serves to load data from file.
// loads data only if load function is called or
// free cache destroys all the data
// data that is used in the engine will be freed by garbage collector only if it is unused (like actual meshes)

class Data3DFile
{
public:
    EXPORT Data3DFile(const std::string &path, bool bLoadMeshData = true);

    EXPORT Mesh *getAsMesh();
    EXPORT Spline *getSplineByName(const std::string &name, SplineInterpolation interpolation = SplineInterpolation::Linear, bool bIsLooped = false);

    EXPORT std::vector<MeshObject *> *getMeshObjectList();
    EXPORT std::vector<Animation *> *getAnimationsList();

    EXPORT void load();
    EXPORT void freeCache();

protected:
    std::vector<MeshObject *> meshObjectList;
    std::vector<Animation *> animationsList;
    Mesh *mesh = nullptr;

    std::string path;
    bool bLoaded = false;
    bool bLoadMeshData = true;
};
