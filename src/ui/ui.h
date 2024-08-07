// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/renderer.h"
#include "window/window.h"
#include "data/entity.h"
#include "data/inputProvider.h"
#include "uiContext.h"
#include "uiNode.h"
#include "uiEventController.h"
#include <vector>

class UI
{
public:
    EXPORT UI(Window *window, Renderer *renderer, Font *defaultFont);
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

    template <class T, typename std::enable_if<std::is_base_of<UIEventController, T>::value>::type * = nullptr>
    EXPORT T *createEventController()
    {
        auto newEventController = new T();
        prepareNewEventController(newEventController);
        return newEventController;
    }

    EXPORT void processMouseClick(float value);

    EXPORT void triggerClick();
    EXPORT void triggerRelease();
    EXPORT void triggerEvent(UIEvent ev, UINode *node);

    float interfaceZoom = 1.0f;

protected:
    EXPORT void prepareNewEventController(UIEventController *eventController);
    EXPORT float getNextWordWidth(std::u32string &str, Font *font, unsigned int position, unsigned int fontSize, float letterSpacing);

    UIContext *uiContext;
    UINode *root;
    InputProvider *inputProvider;
    std::vector<UIEventController *> eventControllers;

    UIRenderBlock *hoveredBlock = nullptr;
    unsigned int clickInputIndex = 0;

    bool clickHappened = false;
    bool releaseHappened = false;
};