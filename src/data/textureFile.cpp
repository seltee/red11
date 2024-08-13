// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "textureFile.h"
#include "utils/image/stb_image.h"
#include "renderer/renderer.h"

TextureFile::TextureFile(std::string sName, std::string sFilePath) : Texture(sName, TextureType::Normal)
{
    this->sFilePath = sFilePath;
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
    if (!nWidth)
        getBufferData();
    return nWidth;
}

int TextureFile::getHeight()
{
    if (!nHeight)
        getBufferData();
    return nHeight;
}

bool TextureFile::isLoaded()
{
    return bLoaded;
}

void TextureFile::load()
{
    if (!bLoaded)
    {
        bLoaded = true;
        int nC;
        data = stbi_load(sFilePath.c_str(), &nWidth, &nHeight, &nC, 4);
    }
}

void TextureFile::unload()
{
    if (bLoaded)
    {
        bLoaded = false;
        stbi_image_free(data);
        data = nullptr;
        Renderer::removeFromAllTextureByIndex(unIndex);
    }
}
