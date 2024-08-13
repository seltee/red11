// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "directx9materialRenderData.h"
#include "directx9data.h"

Directx9MaterialRenderData::Directx9MaterialRenderData(LPDIRECT3DDEVICE9 d3ddev, Material *material)
{
    this->material = material;
    this->d3ddev = d3ddev;
}

DX9Material *Directx9MaterialRenderData::getData()
{
    if (lastUpdIndex != this->material->getUpdateIndex())
        rebuildData();
    return &data;
}

void Directx9MaterialRenderData::setupForRender(Directx9data *dxData, bool bUseReflectionRadiance)
{
    MaterialSimple *materialSimple = reinterpret_cast<MaterialSimple *>(material);
    materialSimple->load();

    Directx9TextureRenderData *albedoTextureData = materialSimple->getAlbedoTexture() ? dxData->getTextureRenderData(materialSimple->getAlbedoTexture()) : nullptr;
    Directx9TextureRenderData *alphaTextureData = materialSimple->getAlphaTexture() ? dxData->getTextureRenderData(materialSimple->getAlphaTexture()) : nullptr;
    Directx9TextureRenderData *emissionTextureData = materialSimple->getEmissionTexture() ? dxData->getTextureRenderData(materialSimple->getEmissionTexture()) : nullptr;
    Directx9TextureRenderData *normalTextureData = materialSimple->getNormalTexture() ? dxData->getTextureRenderData(materialSimple->getNormalTexture()) : nullptr;
    Directx9TextureRenderData *metallicTextureData = materialSimple->getMetallicTexture() ? dxData->getTextureRenderData(materialSimple->getMetallicTexture()) : nullptr;
    Directx9TextureRenderData *roughnessTextureData = materialSimple->getRoughnessTexture() ? dxData->getTextureRenderData(materialSimple->getRoughnessTexture()) : nullptr;
    Directx9TextureRenderData *aoTextureData = materialSimple->getAOTexture() ? dxData->getTextureRenderData(materialSimple->getAOTexture()) : nullptr;

    // === Setup material data ===
    Directx9MaterialRenderData *materialRenderData = dxData->getMaterialRenderData(material);
    DX9Material *data = materialRenderData->getData();
    data->useReflectionRadiance = bUseReflectionRadiance ? 1.0f : 0.0f;
    d3ddev->SetPixelShaderConstantF(12, (const float *)data, 3);

    // === Setup colors ===
    // Albedo Color
    d3ddev->SetPixelShaderConstantF(15, materialSimple->getAlbedoColor().getAsFloatArray(), 1);

    // Emission Color
    d3ddev->SetPixelShaderConstantF(16, materialSimple->getEmissionColor().getAsFloatArray(), 1);

    // === Setup textures ===
    if (albedoTextureData)
        d3ddev->SetTexture(0, albedoTextureData->texture);
    if (alphaTextureData)
        d3ddev->SetTexture(1, alphaTextureData->texture);
    if (emissionTextureData)
        d3ddev->SetTexture(2, emissionTextureData->texture);
    if (normalTextureData)
        d3ddev->SetTexture(3, normalTextureData->texture);
    if (metallicTextureData)
        d3ddev->SetTexture(4, metallicTextureData->texture);
    if (roughnessTextureData)
        d3ddev->SetTexture(5, roughnessTextureData->texture);
    if (aoTextureData)
        d3ddev->SetTexture(6, aoTextureData->texture);

    switch (materialSimple->getDisplay())
    {
    case MaterialDisplay::Alpha:
        d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
        d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;

    default:
        d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        break;
    }
}

void Directx9MaterialRenderData::setupForDepth(Directx9data *dxData)
{
    MaterialSimple *materialSimple = reinterpret_cast<MaterialSimple *>(material);

    Directx9TextureRenderData *albedoTextureData = materialSimple->getAlbedoTexture() ? dxData->getTextureRenderData(materialSimple->getAlbedoTexture()) : nullptr;
    Directx9TextureRenderData *alphaTextureData = materialSimple->getAlphaTexture() ? dxData->getTextureRenderData(materialSimple->getAlphaTexture()) : nullptr;

    // === Setup material data ===
    Directx9MaterialRenderData *materialRenderData = dxData->getMaterialRenderData(material);
    d3ddev->SetPixelShaderConstantF(12, (const float *)materialRenderData->getData(), 3);

    // === Setup colors ===
    // Albedo Color
    d3ddev->SetPixelShaderConstantF(15, materialSimple->getAlbedoColor().getAsFloatArray(), 1);

    // === Setup textures ===
    if (albedoTextureData)
        d3ddev->SetTexture(0, albedoTextureData->texture);
    if (alphaTextureData)
        d3ddev->SetTexture(1, alphaTextureData->texture);
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
    data.useReflectionRadiance = 0.0f;

    data.roughnessValue = materialSimple->getRoughness();
    data.metallicValue = materialSimple->getMetallic();
    data.alphaValue = materialSimple->getAlpha();
    data.alphaHalfQuite = materialSimple->getDisplay() == MaterialDisplay::SolidMask ? 1.0f : 0.0f;
}