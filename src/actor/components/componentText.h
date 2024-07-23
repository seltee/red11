// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "component.h"
#include "data/material/materialSimple.h"
#include "data/font.h"

struct ComponentTextLetterItem
{
    MaterialSimple *material;
    int code, size;
    float localWidth;
    float localHeight;
    float localScale;
};

class ComponentText : public Component
{
public:
    EXPORT ComponentText();

    EXPORT void onRenderQueue(Renderer *renderer) override final;

    EXPORT void setFont(Font *font);
    EXPORT void setSize(int size);
    EXPORT void setText(std::string text);

protected:
    EXPORT void rebuild();
    EXPORT void destroyMatCache();
    EXPORT int getCacheLetterIndex(int code, int size);

    std::string text = "";
    Font *font = nullptr;
    int size;
    bool bIsDirty = true;

    std::vector<ComponentTextLetterItem> lettersCache;
    std::vector<int> lettersList;

    static Mesh *mesh;
};
