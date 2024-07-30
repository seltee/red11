#include "resourceManager.h"
#include "data/material/material.h"
#include "data/texture.h"
#include "data/sound.h"
#include "data/mesh.h"

void ResourceManager::freeUnusedAll()
{
    freeUnusedMaterials();
    freeUnusedMeshes();
    freeUnusedTextures();
    freeUnusedSounds();
}

void ResourceManager::freeUnusedMaterials()
{
    std::vector<Material *> *list = Material::getMaterialsList();
    for (auto &mat : *list)
    {
        if (mat->isLoaded() && mat->getAmountOfUsers() == 0)
        {
            mat->unload();
        }
    }
}

void ResourceManager::freeUnusedMeshes()
{
    std::vector<Mesh *> *list = Mesh::getMeshList();
    for (auto &mesh : *list)
    {
        if (mesh->isLoaded() && mesh->getAmountOfUsers() == 0)
        {
            mesh->unload();
        }
    }
}

void ResourceManager::freeUnusedTextures()
{
    std::vector<Texture *> *list = Texture::getTextureList();
    for (auto &texture : *list)
    {
        if (texture->isLoaded() && texture->getAmountOfUsers() == 0)
        {
            texture->unload();
        }
    }
}

void ResourceManager::freeUnusedSounds()
{
    std::vector<Sound *> *list = Sound::getSoundsList();
    for (auto &sound : *list)
    {
        if (sound->isLoaded() && sound->getAmountOfUsers() == 0)
        {
            sound->unload();
        }
    }
}
