// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "data/texture.h"
#include <string>
#include <vector>

#define MAP_AMOUNT (256 * 256)
#define MAP_MAX_SIZE 1024

struct Glyph
{
    Texture *texture;
    unsigned int code;
    unsigned int size;
    int shiftX, shiftY;
    unsigned int w, h;
};

class Font
{
public:
    EXPORT Font(const std::string &path);

    EXPORT Glyph *getGlyph(unsigned int code, unsigned int size);
    EXPORT Texture *getGlyphTexture(unsigned int code, unsigned int size);

    EXPORT unsigned int measureWidth(const std::string &string, unsigned int size);
    EXPORT unsigned int measureHeight(const std::string &string, unsigned int size);

    inline bool isReady() { return bIsReady; }

protected:
    int createGlyphInList(unsigned int code, unsigned int size);

    bool bIsReady = false;
    std::string path;

    void *fontInfo;

    int *map[MAP_MAX_SIZE];
    std::vector<Glyph> list;
};