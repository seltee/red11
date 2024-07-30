// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "textureFile.h"
#include "utils/image/stb_image.h"
#include "renderer/renderer.h"

TextureFile::TextureFile(std::string name, std::string filePath) : Texture(name, TextureType::Normal)
{
    this->filePath = filePath;
    this->bStaticBuffer = false;
}

unsigned char *TextureFile::getBufferData()
{
    load();
    return data;
}

unsigned char *TextureFile::setBufferSize(int width, int height)
{
    return nullptr;
}

int TextureFile::getWidth()
{
    if (!width)
        getBufferData();
    return width;
}

int TextureFile::getHeight()
{
    if (!height)
        getBufferData();
    return height;
}

bool TextureFile::isLoaded()
{
    return loaded;
}

void TextureFile::load()
{
    if (!loaded)
    {
        loaded = true;
        int c;
        data = stbi_load(filePath.c_str(), &width, &height, &c, 4);
    }
}

void TextureFile::unload()
{
    if (loaded)
    {
        loaded = false;
        stbi_image_free(data);
        data = nullptr;
        Renderer::removeFromAllTextureByIndex(index);
    }
}
