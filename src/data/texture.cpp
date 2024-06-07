// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "texture.h"

std::list<Texture *> Texture::textures;

Texture::Texture(std::string name)
{
    this->name = name;
    static unsigned int lastIndex = 0;
    index = lastIndex;
    lastIndex++;

    textures.push_back(this);
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
    updIndex++;
}

unsigned char *Texture::setBuffer(int width, int height)
{
    if (data)
        delete[] data;

    this->width = width;
    this->height = height;

    int size = width * height * 4;
    data = new unsigned char[size];

    memset(data, 0, size);
    updIndex++;
    return data;
}

int Texture::getWidth()
{
    return width;
}

int Texture::getHeight()
{
    return height;
}

unsigned int Texture::getColorAtPoint(int x, int y)
{
    auto dt = getBufferData();
    if (dt && width && height)
    {
        int p = (y * width + x);
        if (p >= 0 && p < width * height)
            return reinterpret_cast<unsigned int *>(dt)[p];
    }
    return 0;
}