// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "componentText.h"
#include "red11.h"
#include <string>
#include <locale>
#include <codecvt>

#define COMPONENT_TEXT_MAX_MATERIALS 128

Mesh *ComponentText::mesh = nullptr;

ComponentText::ComponentText()
{
    if (!mesh)
    {
        mesh = Red11::getMeshBuilder()->createPlain(0.1f);
        mesh->addUser();
    }
}

void ComponentText::onRenderQueue(Renderer *renderer)
{
    if (bVisible)
    {
        if (bIsDirty)
            rebuild();

        if (mesh)
        {
            Entity t;
            t.setPosition(0.05, 0, 0.05);
            float shift = 0.0f;

            for (auto &letterIndex : lettersList)
            {
                ComponentTextLetterItem *item = &lettersCache.at(letterIndex);
                if (item->localWidth > 0.001f)
                {
                    t.setScale(item->localScale);
                    t.setPosition(Vector3(shift + 0.05f * item->localScale, 0, 0.05f * item->localScale - item->localHeight));

                    Matrix4 model = getModelMatrix() * t.getModelMatrix();
                    renderer->queueMesh(mesh, item->material, model);
                    t.translate(Vector3(item->localWidth, 0, 0));

                    shift += item->localWidth;
                }
                else
                {
                    shift += 0.04f;
                }
            }
        }
    }
}

void ComponentText::setFont(Font *font)
{
    this->font = font;
    destroyMatCache();
    bIsDirty = true;
}

void ComponentText::setSize(int size)
{
    this->size = size;
    bIsDirty = true;
}

void ComponentText::setText(const std::string &text)
{
    this->text = text;
    bIsDirty = true;
}

void ComponentText::rebuild()
{
    bIsDirty = false;
    if (lettersCache.size() > COMPONENT_TEXT_MAX_MATERIALS)
        destroyMatCache();

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    std::u32string text32 = convert.from_bytes(text);

    lettersList.clear();

    for (auto &c : text32)
    {
        lettersList.push_back(getCacheLetterIndex(c, size));
    }
}

void ComponentText::destroyMatCache()
{
    for (auto &item : lettersCache)
    {
        delete item.material;
    }
}

int ComponentText::getCacheLetterIndex(int code, int size)
{
    int index = 0;
    for (auto &letter : lettersCache)
    {
        if (letter.code == code && letter.size == size)
            return index;
        index++;
    }

    index = lettersCache.size();
    Glyph *glyph = font->getGlyph(code, size);

    auto material = new MaterialSimple();
    material->setAlphaTexture(glyph->texture);
    material->setDisplayMode(MaterialDisplay::Alpha);
    material->setEmissionColor(Color(0.9f, 0.9f, 0.9f));

    float textureSize = static_cast<float>(glyph->texture->getWidth());
    float localWidth = (static_cast<float>(glyph->w) / 128.0f) * 0.1f;
    float localHeight = (static_cast<float>(glyph->h) / 128.0f) * 0.1f;
    float localScale = textureSize / 128.0f;

    lettersCache.push_back({material, code, size, localWidth, localHeight, localScale});

    return index;
}