#pragma once
#include "utils/utils.h"

class ResourceManager
{
public:
    EXPORT void freeUnusedAll();

    EXPORT void freeUnusedMaterials();
    EXPORT void freeUnusedMeshes();
    EXPORT void freeUnusedTextures();
    EXPORT void freeUnusedSounds();
};
