// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include <list>
#include <string>

enum class TextureType
{
    Normal,
    GpuStencil
};

class Texture
{
public:
    EXPORT Texture(TextureType textureType);
    EXPORT Texture(std::string name, TextureType textureType);
    EXPORT Texture(std::string name, TextureType textureType, int width, int height, unsigned char *data);
    EXPORT virtual ~Texture();

    EXPORT virtual unsigned char *getBufferData();
    EXPORT virtual void releaseBuffer();
    EXPORT virtual unsigned char *setBuffer(int width, int height);
    EXPORT virtual int getWidth();
    EXPORT virtual int getHeight();

    EXPORT unsigned int getColorAtPoint(int x, int y);

    EXPORT void setGpuRenderSize(int width, int height);

    EXPORT virtual void destroy();

    inline unsigned int getIndex() { return index; }
    inline unsigned int getUpdIndex() { return updIndex; }
    inline void markUpdated() { updIndex++; }
    inline void markAsStatic(bool state) { bStaticBuffer = state; }
    inline bool isStaticBuffer() { return bStaticBuffer; }

    inline TextureType getType() { return textureType; }
    inline std::string &getName() { return name; }

    static inline std::list<Texture *> *getTextureList() { return &textures; }

protected:
    std::string name;
    TextureType textureType = TextureType::Normal;

    int index = 0;
    int updIndex = 0;

    unsigned char *data = nullptr;
    int width = 0, height = 0;

    // static buffer can't be removed to free space
    // if getBufferData always generates correct data even if it was released by space collector
    // mark it as false - then engine will release this data if memory needed
    bool bStaticBuffer = true;

    static std::list<Texture *> textures;
};