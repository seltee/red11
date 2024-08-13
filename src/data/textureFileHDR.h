// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "texture.h"

class TextureFileHDR : public Texture
{
public:
    EXPORT TextureFileHDR(std::string sName, std::string sFilePath, float fLdrScale = 1.0f, float fLdrGamma = 1.0f);

    EXPORT unsigned char *getBufferData() override;
    EXPORT unsigned char *setBufferSize(int nWidth, int nHeight) override;
    EXPORT int getWidth() override;
    EXPORT int getHeight() override;

    EXPORT bool isLoaded() override;
    EXPORT void load() override;
    EXPORT void unload() override;

    EXPORT Texture *getRadianceTexture();

protected:
    EXPORT Texture *createAmbinetTexture();

    std::string sFilePath;
    bool bLoaded = false;
    float fLdrScale, fLdrGamma;

    Texture *radianceTexture = nullptr;
};