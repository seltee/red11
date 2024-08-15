// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "texture.h"
#include "renderer/renderer.h"

std::vector<Texture *> Texture::textures;
bool Texture::pbIndexPool[MAX_ELEMENT_INDEX];
unsigned int Texture::unNextIndex = 0;

Texture::Texture(TextureType textureType) : Texture("texture", textureType)
{
}

Texture::Texture(const std::string &sName, TextureType textureType)
{
    this->sName = sName;
    this->textureType = textureType;
    unIndex = getNextIndex();
    textures.push_back(this);
}

Texture::Texture(const std::string &sName, TextureType textureType, int nWidth, int nHeight, unsigned char *data) : Texture(sName, textureType)
{
    this->textureType = textureType;
    setBufferSize(nWidth, nHeight);
    memcpy(this->data, data, nWidth * nHeight * getBytesPerPixel());
}

Texture::~Texture()
{
    releaseBuffer();

    auto it = textures.begin();
    while (it != textures.end())
    {
        if ((*it) == this)
        {
            textures.erase(it);
            break;
        }
        it++;
    }

    pbIndexPool[unIndex] = false;
}

unsigned char *Texture::getBufferData()
{
    return data;
}

void Texture::releaseBuffer()
{
    if (data)
        delete[] data;
    data = nullptr;
    unUpdIndex++;
}

unsigned char *Texture::setBufferSize(int nWidth, int nHeight)
{
    if (data)
        delete[] data;

    this->nWidth = nWidth;
    this->nHeight = nHeight;

    int size = nWidth * nHeight * getBytesPerPixel();
    data = new unsigned char[size];

    memset(data, 0, size);
    unUpdIndex++;
    return data;
}

int Texture::getWidth()
{
    return nWidth;
}

int Texture::getHeight()
{
    return nHeight;
}

int Texture::getBytesPerPixel()
{
    return textureType == TextureType::ByteMap ? 1 : 4;
}

unsigned int Texture::getColorAtPoint(int x, int y)
{
    auto dt = getBufferData();
    if (dt && nWidth && nHeight)
    {
        int p = (y * nWidth + x);
        if (p >= 0 && p < nWidth * nHeight)
            return reinterpret_cast<unsigned int *>(dt)[p];
    }
    return 0;
}

void Texture::setGpuRenderSize(int nWidth, int nHeight)
{
    this->nWidth = nWidth;
    this->nHeight = nHeight;
}

void Texture::destroy()
{
    delete this;
}

bool Texture::isLoaded()
{
    return true;
}

void Texture::load()
{
}

void Texture::unload()
{
    Renderer::removeFromAllTextureByIndex(unIndex);
}

unsigned int Texture::getNextIndex()
{
    while (pbIndexPool[unNextIndex])
    {
        unNextIndex++;
        if (unNextIndex >= MAX_ELEMENT_INDEX)
            unNextIndex = 0;
    }

    pbIndexPool[unNextIndex] = true;
    return unNextIndex;
}