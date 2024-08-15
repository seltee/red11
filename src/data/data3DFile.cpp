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