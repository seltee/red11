// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/usable.h"
#include "settings.h"
#include <list>
#include <string>

enum class TextureType
{
    Normal,
    ByteMap,
    GpuStencil
};

class Texture : public Usable
{
public:
    EXPORT Texture(TextureType textureType);
    EXPORT Texture(std::string sName, TextureType textureType);
    EXPORT Texture(std::string sName, TextureType textureType, int nWidth, int nHeight, unsigned char *data);
    EXPORT virtual ~Texture();

    EXPORT virtual unsigned char *getBufferData();
    EXPORT virtual void releaseBuffer();
    EXPORT virtual unsigned char *setBufferSize(int nWidth, int nHeight);
    EXPORT virtual int getWidth();
    EXPORT virtual int getHeight();
    EXPORT int getBytesPerPixel();

    EXPORT unsigned int getColorAtPoint(int nX, int nY);

    EXPORT void setGpuRenderSize(int nWidth, int nHeight);

    EXPORT virtual void destroy();

    inline unsigned int getIndex() { return unIndex; }
    inline unsigned int getUpdIndex() { return unUpdIndex; }
    inline void markUpdated() { unUpdIndex++; }
    inline void markAsStatic(bool bState) { bStaticBuffer = bState; }
    inline bool isStaticBuffer() { return bStaticBuffer; }

    inline TextureType getType() { return textureType; }
    inline std::string &getName() { return sName; }

    static inline std::vector<Texture *> *getTextureList() { return &textures; }

    EXPORT bool isLoaded() override;
    EXPORT void load() override;
    EXPORT void unload() override;

protected:
    unsigned int getNextIndex();

    std::string sName;
    TextureType textureType = TextureType::Normal;

    unsigned int unIndex = 0;
    unsigned int unUpdIndex = 0;

    unsigned char *data = nullptr;
    int nWidth = 0, nHeight = 0;

    // static buffer can't be removed to free space
    // if getBufferData always generates correct data even if it was released by space collector
    // mark it as false - then engine will release this data if memory needed
    bool bStaticBuffer = true;

    static std::vector<Texture *> textures;

    // Index pool
    static bool pbIndexPool[MAX_ELEMENT_INDEX];
    static unsigned int unNextIndex;
};