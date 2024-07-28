// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "font.h"
#include "utils/font/stb_truetype.h"
#include <stdio.h>
#include <string>
#include <locale>
#include <codecvt>

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

unsigned int Font::measureWidth(std::string string, unsigned int size)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    std::u32string text32 = convert.from_bytes(string);

    int width = 0.0f;
    for (auto &c : text32)
    {
        Glyph *glyph = getGlyph(c, size);
        width += glyph->w;
    }
    return width;
}

unsigned int Font::measureHeight(std::string string, unsigned int size)
{
    return size;
}

int Font::createGlyphInList(unsigned int code, unsigned int size)
{
    stbtt_fontinfo *font = (stbtt_fontinfo *)fontInfo;

    float scale = stbtt_ScaleForPixelHeight(font, size);
    int sw, sh, width, height;
    stbtt_GetCodepointBitmapBox(font, code, scale, scale, &sw, &sh, &width, &height);
    width += sw;
    height -= sh;

    int textureWidth = nextPowerOfTwo(width + 2);
    int textureHeight = nextPowerOfTwo(height + 2);

    int textureSize = textureWidth > textureHeight ? textureWidth : textureHeight;

    unsigned char *bitmap = new unsigned char[textureSize * textureSize];
    memset(bitmap, 0, textureSize * textureSize);
    stbtt_MakeCodepointBitmap(font, bitmap, textureSize, textureSize, textureSize, scale, scale, code);
    for (int iy = textureSize - 1; iy >= 0; iy--)
    {
        int shiftY = iy * textureSize;
        for (int ix = textureSize - 1; ix >= 0; ix--)
        {
            int shift = shiftY + ix;
            if (ix == 0 || iy == 0)
                bitmap[shift] = 0;
            else
                bitmap[shift] = bitmap[shift - 1 - textureSize];
        }
    }
    Texture *texture = new Texture("Glyph", TextureType::ByteMap, textureSize, textureSize, bitmap);
    delete[] bitmap;

    // printf("Glyph %c - %i %i %i %i\n", code, width, height, sw, sh);

    list.push_back({texture, code, size, sw - 1, sh - 1, static_cast<unsigned int>(width <= 1 ? size * 2 / 5 : width), static_cast<unsigned int>(height)});

    return list.size() - 1;
}