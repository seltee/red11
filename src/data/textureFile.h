// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "texture.h"

class TextureFile : public Texture
{
public:
    EXPORT TextureFile(std::string name, std::string filePath);

    EXPORT unsigned char *getBufferData() override;
    EXPORT unsigned char *setBufferSize(int width, int height) override;
    EXPORT int getWidth() override;
    EXPORT int getHeight() override;

    EXPORT Texture *getFileDataAsTexture(std::string name);


protected:
    std::string filePath;
};