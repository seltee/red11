// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "data3DFile.h"
#include "utils/FBX/loaderFBX.h"
#include "utils/meshCombiner.h"

Data3DFile::Data3DFile(const std::string &path, bool bLoadMeshData)
{
    this->path = path;
    this->bLoadMeshData = bLoadMeshData;
}

Mesh *Data3DFile::getAsMesh()
{
    if (!bLoaded)
        load();
    if (mesh)
        return mesh;

    if (bLoaded && meshObjectList.size() > 0)
    {
        if (meshObjectList.size() == 1)
        {
            printf("Compiling single mesh\n");
            mesh = meshObjectList.at(0)->getTransformedMesh();
        }
        else
        {
            printf("Compiling multi mesh out of %i meshes\n", (int)meshObjectList.size());
            mesh = combineMeshList(meshObjectList);
        }
    }
    return mesh;
}

Spline *Data3DFile::getSplineByName(const std::string &name, SplineInterpolation interpolation, bool bIsLooped)
{
    if (!bLoaded)
        load();
    for (auto &object : meshObjectList)
    {
        if (*object->getNamePointer() == name)
        {
            printf("Mesh found %i\n", object->getVerticesAmount());
            return new Spline(object->getVertices(), object->getVerticesAmount(), interpolation, bIsLooped);
        }
    }
    return nullptr;
}

std::vector<MeshObject *> *Data3DFile::getMeshObjectList()
{
    if (!bLoaded)
        load();
    if (bLoaded)
        return &meshObjectList;
    return nullptr;
}

std::vector<Animation *> *Data3DFile::getAnimationsList()
{
    if (!bLoaded)
        load();
    if (bLoaded)
        return &animationsList;
    return nullptr;
}

MeshObject *Data3DFile::getMeshObjectByName(std::string name)
{
    if (!bLoaded)
        load();
    if (bLoaded)
    {
        for (auto &mesh : meshObjectList)
        {
            if (*mesh->getNamePointer() == name)
                return mesh;
        }
    }
    return nullptr;
}

Animation *Data3DFile::getAnimationByName(std::string name)
{
    if (!bLoaded)
        load();
    if (bLoaded)
    {
        for (auto &animation : animationsList)
        {
            if (animation->getName() == name)
                return animation;
        }
    }
    return nullptr;
}

void Data3DFile::load()
{
    if (LoaderFBX::loadFBXFile(path, bLoadMeshData ? &meshObjectList : nullptr, &animationsList))
        bLoaded = true;
    else
        printf("Unable to load %s\n", path.c_str());
}

void Data3DFile::freeCache()
{
}