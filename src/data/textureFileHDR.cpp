// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "textureFileHDR.h"
#include "utils/image/stb_image.h"
#include "renderer/renderer.h"
#include "red11.h"

TextureFileHDR::TextureFileHDR(std::string sName, std::string sFilePath, float fLdrScale, float fLdrGamma) : Texture(sName, TextureType::Normal)
{
    this->sFilePath = sFilePath;
    this->bStaticBuffer = false;
    this->fLdrScale = fLdrScale;
    this->fLdrGamma = fLdrGamma;
}

unsigned char *TextureFileHDR::getBufferData()
{
    load();
    return data;
}

unsigned char *TextureFileHDR::setBufferSize(int nWidth, int nHeight)
{
    return nullptr;
}

int TextureFileHDR::getWidth()
{
    if (!nWidth)
        getBufferData();
    return nWidth;
}

int TextureFileHDR::getHeight()
{
    if (!nHeight)
        getBufferData();
    return nHeight;
}

bool TextureFileHDR::isLoaded()
{
    return bLoaded;
}

void TextureFileHDR::load()
{
    if (!bLoaded)
    {
        bLoaded = true;

        stbi_hdr_to_ldr_gamma(fLdrScale);
        stbi_hdr_to_ldr_scale(fLdrGamma);

        int nC;
        data = stbi_load(sFilePath.c_str(), &nWidth, &nHeight, &nC, 4);

        radianceTexture = createAmbinetTexture();
    }
}

void TextureFileHDR::unload()
{
    if (bLoaded)
    {
        bLoaded = false;
        stbi_image_free(data);
        data = nullptr;
        Renderer::removeFromAllTextureByIndex(unIndex);
    }
}

Texture *TextureFileHDR::getRadianceTexture()
{
    if (!bLoaded)
        load();
    return radianceTexture;
}

Texture *TextureFileHDR::createAmbinetTexture()
{
    Texture *textureOut = nullptr;
    if (data)
    {
        // TODO: Blur through shader
        // This code is not very fast
        // TODO: Process in threads at least
        const int nWidthBlured = 128;
        const int nHeightBlured = 128;
        float ratioWidth = static_cast<float>(nWidth) / static_cast<float>(nWidthBlured);
        float ratioHeight = static_cast<float>(nHeight) / static_cast<float>(nHeightBlured);
        unsigned char *rData = (unsigned char *)malloc(nWidthBlured * nHeightBlured * 4);

        for (int iy = 0; iy < nHeightBlured; iy++)
        {
            for (int ix = 0; ix < nWidthBlured; ix++)
            {
                int shift = (ix + iy * nWidthBlured) * 4;
                int originalX = static_cast<int>(static_cast<float>(ix) * ratioWidth) % nWidth;
                int originalY = static_cast<int>(static_cast<float>(iy) * ratioHeight) % nHeight;
                int originalShift = (originalX + originalY * nWidth) * 4;

                rData[shift + 0] = data[originalShift + 0];
                rData[shift + 1] = data[originalShift + 1];
                rData[shift + 2] = data[originalShift + 2];
                rData[shift + 3] = 255;
            }
        }

        unsigned char *blurData = (unsigned char *)malloc(nWidthBlured * nHeightBlured * 4);
        int pickSize = 16;
        int pickSize2 = (pickSize + pickSize + 1) * (pickSize + pickSize + 1);

        for (int iy = 0; iy < nHeightBlured; iy++)
        {
            for (int ix = 0; ix < nWidthBlured; ix++)
            {
                unsigned int mData[3] = {0, 0, 0};
                int sShift = ix + iy * nWidthBlured;

                for (int pickY = -pickSize; pickY <= pickSize; pickY++)
                {
                    int linePick = (iy + pickY);
                    if (linePick >= nHeightBlured)
                        linePick = nHeightBlured - 1;
                    if (linePick < 0)
                        linePick = 0;

                    int linePickShift = linePick * nWidthBlured;

                    for (int pickX = -pickSize; pickX <= pickSize; pickX++)
                    {
                        int pixelPick = ix + pickX;
                        if (pixelPick < 0)
                            pixelPick += nWidthBlured;
                        if (pixelPick >= nWidthBlured)
                            pixelPick -= nWidthBlured;
                        int fullShift = ((linePickShift) + pixelPick) * 4;
                        mData[0] += rData[fullShift];
                        mData[1] += rData[fullShift + 1];
                        mData[2] += rData[fullShift + 2];
                    }
                }
                mData[0] /= pickSize2;
                mData[1] /= pickSize2;
                mData[2] /= pickSize2;
                blurData[sShift * 4] = mData[0];
                blurData[sShift * 4 + 1] = mData[1];
                blurData[sShift * 4 + 2] = mData[2];
                blurData[sShift * 4 + 3] = 255;
            }
        }

        textureOut = new Texture("Radiance", TextureType::Normal, nWidthBlured, nHeightBlured, blurData);

        free(rData);
        free(blurData);
    }
    else
    {
        Red11::getLogger()->logFileAndConsole("ERROR: %s couldn't be loaded", sFilePath.c_str());
    }
    return textureOut;
}