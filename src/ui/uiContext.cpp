// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "uiContext.h"
#include <algorithm>

// Comparison function to sort UIRenderBlock
bool compareByIndex(const UIRenderBlock &a, const UIRenderBlock &b)
{
    return a.index < b.index;
}

UIContext::UIContext(Window *window, Renderer *renderer, Font *defaultFont)
{
    this->window = window;
    this->renderer = renderer;
    this->defaultFont = defaultFont;

    memset(blocks, 0, sizeof(UIRenderBlock) * MAX_UI_RENDER_BLOCKS);
}

void UIContext::cleanForNewRender()
{
    blocksCount = 0;
    xPosition = 0;
    yPosition = 0;
    parentBlock = nullptr;
    initialBlock = getBlock();
    initialBlock->x = 0.0f;
    initialBlock->y = 0.0f;
    initialBlock->source = nullptr;
    rootBlock = initialBlock;
    index = 0;
}

void UIContext::setParentData(float xPosition, float yPosition, UIRenderBlock *parentBlock, int index)
{
    this->xPosition = xPosition;
    this->yPosition = yPosition;
    this->parentBlock = parentBlock;
    this->index = index;
}

UIRenderBlock *UIContext::getBlock()
{
    if (blocksCount < MAX_UI_RENDER_BLOCKS)
    {
        blocksCount++;
        blocks[blocksCount - 1].children.clear();
        return &blocks[blocksCount - 1];
    }
    return nullptr;
}

void UIContext::sortBlocks()
{
    std::sort(blocks, blocks + blocksCount, compareByIndex);
}