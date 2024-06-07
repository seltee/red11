// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "textureFile.h"
#include "utils/image/stb_image.h"

TextureFile::TextureFile(std::string name, std::string filePath) : Texture(name)
{
    this->filePath = filePath;
    this->bStaticBuffer = false;
}

unsigned char *TextureFile::getBufferData()
{
    if (!data)
    {
        int c;
        data = stbi_load(filePath.c_str(), &width, &height, &c, 4);
        printf("%s loaded - %i/%i - %i\n", filePath.c_str(), width, height, c);
    }
    return data;
}

unsigned char *TextureFile::setBuffer(int width, int height)
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

Texture *TextureFile::getFileDataAsTexture(std::string name)
{
    if (getBufferData() || getWidth() || getHeight())
    {
        Texture *texture = new Texture(name);
        texture->setBuffer(getWidth(), getHeight());
        unsigned char *copiedData = texture->getBufferData();
        memcpy(copiedData, data, getWidth() * getHeight() * 4);
        return texture;
    }
    return nullptr;
}
