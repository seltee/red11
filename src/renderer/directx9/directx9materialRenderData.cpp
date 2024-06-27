// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9materialRenderData.h"

Directx9MaterialRenderData::Directx9MaterialRenderData(Material *material)
{
    this->material = material;
}

DX9Material *Directx9MaterialRenderData::getData()
{
    if (lastUpdIndex != this->material->getUpdateIndex())
        rebuildData();
    return &data;
}

void Directx9MaterialRenderData::rebuildData()
{
    MaterialSimple *materialSimple = reinterpret_cast<MaterialSimple *>(material);
    lastUpdIndex = this->material->getUpdateIndex();

    data.useAlbedoTexture = materialSimple->getAlbedoTexture() ? 1.0f : 0.0f;
    data.useAlphaTexture = materialSimple->getAlphaTexture() ? 1.0f : 0.0f;
    data.useNormalTexture = materialSimple->getNormalTexture() ? 1.0f : 0.0f;
    data.useMetallicTexture = materialSimple->getMetallicTexture() ? 1.0f : 0.0f;

    data.useRoughnessTexture = materialSimple->getRoughnessTexture() ? 1.0f : 0.0f;
    data.useAOTexture = materialSimple->getAOTexture() ? 1.0f : 0.0f;
    data.useEmissionTexture = materialSimple->getEmissionTexture() ? 1.0f : 0.0f;
    data.notUsed1 = 0.0f;

    data.roughnessValue = materialSimple->getRoughness();
    data.metallicValue = materialSimple->getMetallic();
    data.alphaValue = materialSimple->getAlpha();
    data.alphaHalfQuite = materialSimple->getDisplay() == MaterialDisplay::SolidMask ? 1.0f : 0.0f;
}