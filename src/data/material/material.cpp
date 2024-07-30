// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "material.h"
#include "renderer/renderer.h"

std::vector<Material *> Material::materials;

Material::Material()
{
    static unsigned int lastIndex = 0;

    index = lastIndex;
    lastIndex++;
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
}

void Material::unload()
{
    Renderer::removeFromAllMaterialByIndex(index);
}