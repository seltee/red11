// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "font.h"
#include "utils/font/stb_truetype.h"
#include <stdio.h>

Font::Font(std::string path)
{
    this->path = path;
    int fileSize = getFileByteSize(path.c_str());
    if (fileSize == 0)
        return;

    FILE *file = fopen(path.c_str(), "rb");
    if (!file)
        return;

    unsigned char *buffer = new unsigned char[fileSize];
    fread(buffer, fileSize, 1, file);
    fclose(file);

    stbtt_fontinfo *font = new stbtt_fontinfo();
    fontInfo = font;
    stbtt_InitFont(font, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));

    memset(map, 0, MAP_MAX_SIZE * sizeof(Glyph **));
    bIsReady = true;
}

Glyph *Font::getGlyph(unsigned int code, unsigned int size)
{
    if (!bIsReady || size <= 0)
        return nullptr;

    if (code >= 0 && code < MAP_AMOUNT && size < MAP_MAX_SIZE)
    {
        if (!map[size])
        {
            map[size] = new int[MAP_AMOUNT];
            for (int i = 0; i < MAP_AMOUNT; i++)
                map[size][i] = -1;
        }

        if (map[size][code] != -1)
            return &list.at(map[size][code]);

        map[size][code] = createGlyphInList(code, size);
        return &list.at(map[size][code]);
    }

    for (auto &item : list)
    {
        if (item.code == code)
            return &item;
    }

    int glyph = createGlyphInList(code, size);
    return &list.at(glyph);

    return nullptr;
}

Texture *Font::getGlyphTexture(unsigned int code, unsigned int size)
{
    Glyph *glyph = getGlyph(code, size);
    if (glyph)
        return glyph->texture;
    return nullptr;
}

int Font::createGlyphInList(unsigned int code, unsigned int size)
{
    stbtt_fontinfo *font = (stbtt_fontinfo *)fontInfo;

    float scale = stbtt_ScaleForPixelHeight(font, size);
    int sw, sh, width, height;
    stbtt_GetCodepointBitmapBox(font, code, scale, scale, &sw, &sh, &width, &height);
    width -= sw;
    height -= sh;

    int textureWidth = nextPowerOfTwo(width);
    int textureHeight = nextPowerOfTwo(height);

    int textureSize = textureWidth > textureHeight ? textureWidth : textureHeight;

    unsigned char *bitmap = new unsigned char[textureSize * textureSize];
    stbtt_MakeCodepointBitmap(font, bitmap, textureSize, textureSize, textureSize, scale, scale, code);
    Texture *texture = new Texture("Glyph", TextureType::ByteMap, textureSize, textureSize, bitmap);
    delete[] bitmap;

    list.push_back({texture, code, size, width, height});

    return list.size() - 1;
}