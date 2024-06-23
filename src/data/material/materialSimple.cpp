// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "materialSimple.h"

MaterialSimple::MaterialSimple()
{
}

MaterialSimple::MaterialSimple(
    Color albedoColor,
    Color emissionColor,
    float metallic,
    float roughness)
{
    this->albedoColor = albedoColor;
    this->emissionColor = emissionColor;
    this->metallic = metallic;
    this->roughness = roughness;
}

MaterialSimple::MaterialSimple(
    Texture *albedoTexture,
    Texture *normalTexture,
    Texture *emissionTexture,
    Texture *metallicTexture,
    Texture *roughnessTexture,
    Texture *AOTexture)
{
    this->albedoTexture = albedoTexture;
    this->normalTexture = normalTexture;
    this->emissionTexture = emissionTexture;
    this->metallicTexture = metallicTexture;
    this->roughnessTexture = roughnessTexture;
    this->AOTexture = AOTexture;

    this->metallic = this->metallicTexture ? 1.0f : 0.0f;
    this->roughness = this->roughnessTexture ? 1.0f : 0.5f;
    this->emissionColor = this->emissionTexture ? Color(1.0f, 1.0f, 1.0f) : Color(0.0f, 0.0f, 0.0f);
}

MaterialType MaterialSimple::getType()
{
    return MaterialType::Simple;
}

bool MaterialSimple::isUsingNormalMap()
{
    return this->normalTexture ? true : false;
}

void MaterialSimple::setAlbedoColor(Color albedoColor)
{
    this->albedoColor = albedoColor;
    updIndex++;
}

void MaterialSimple::setEmissionColor(Color emissionColor)
{
    this->emissionColor = emissionColor;
    updIndex++;
}

void MaterialSimple::setMetallic(float metallic)
{
    this->metallic = metallic;
    updIndex++;
}

void MaterialSimple::setRoughness(float roughness)
{
    this->roughness = roughness;
    updIndex++;
}

void MaterialSimple::setAlbedoTexture(Texture *texture)
{
    this->albedoTexture = texture;
    updIndex++;
}

void MaterialSimple::setEmissionTexture(Texture *texture)
{
    this->emissionTexture = texture;
    updIndex++;
}

void MaterialSimple::setNormalTexture(Texture *texture)
{
    this->normalTexture = texture;
    updIndex++;
}

void MaterialSimple::setMetallicTexture(Texture *texture)
{
    this->metallicTexture = texture;
    updIndex++;
}

void MaterialSimple::setRoughnessTexture(Texture *texture)
{
    this->roughnessTexture = texture;
    updIndex++;
}

void MaterialSimple::setAOTexture(Texture *texture)
{
    AOTexture = texture;
    updIndex++;
}