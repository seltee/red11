// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/renderer.h"
#include "window/window.h"
#include "uiContext.h"
#include "uiNode.h"
#include "data/entity.h"
#include <vector>
class UI
{
public:
    EXPORT UI(Window *window, Renderer *renderer);
    EXPORT ~UI();

    template <class T, typename std::enable_if<std::is_base_of<UINode, T>::value>::type * = nullptr>
    EXPORT T *createUINodeChild()
    {
        return root->createUINodeChild<T>();
    }

    EXPORT void destroy();
    EXPORT void destroyAllChildren();

    EXPORT void process(float delta);
    EXPORT void render();

protected:
    UIContext *uiContext;
    UINode *root;
};