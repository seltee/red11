// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/renderer.h"
#include "window/window.h"
#include "utils/utils.h"
#include "uiRenderBlock.h"

#define MAX_UI_RENDER_BLOCKS 16 * 1024

class UIContext
{
public:
    EXPORT UIContext(Window *window, Renderer *renderer);

    inline Window *getWindow() { return window; }
    inline Renderer *getRenderer() { return renderer; }

    void cleanForNewRender();
    void setParentData(float xPosition, float yPosition, UIRenderBlock *parentBlock, int index);
    UIRenderBlock *getBlock();
    void sortBlocks();

    float xPosition = 0, yPosition = 0;
    UIRenderBlock *parentBlock = nullptr;
    UIRenderBlock *initialBlock = nullptr;
    UIRenderBlock *rootBlock = nullptr;
    int index = 0;

    inline int getBlocksCount() { return blocksCount; }
    inline UIRenderBlock &getBlock(int index) { return blocks[index]; }

protected:
    Window *window;
    Renderer *renderer;

    int blocksCount = 0;
    UIRenderBlock blocks[MAX_UI_RENDER_BLOCKS];
};
