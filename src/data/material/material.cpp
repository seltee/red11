// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "material.h"
#include "renderer/renderer.h"

std::vector<Material *> Material::materials;
bool Material::indexPool[MAX_ELEMENT_INDEX];
unsigned int Material::nextIndex = 0;

Material::Material()
{
    index = getNextIndex();
    updIndex = 0;
    materials.push_back(this);
}

Material::~Material()
{
    unload();
    auto it = materials.begin();
    while (it != materials.end())
    {
        if ((*it) == this)
        {
            materials.erase(it);
            break;
        }
        it++;
    }
    indexPool[index] = false;
}

void Material::unload()
{
    Renderer::removeFromAllMaterialByIndex(index);
}

unsigned int Material::getNextIndex()
{
    while (indexPool[nextIndex])
    {
        nextIndex++;
        if (nextIndex >= MAX_ELEMENT_INDEX)
            nextIndex = 0;
    }

    indexPool[nextIndex] = true;
    return nextIndex;
}