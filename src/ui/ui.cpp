// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui.h"

UI::UI(Window *window, Renderer *renderer)
{
    uiContext = new UIContext(window, renderer);
    root = new UINode(nullptr);
}

UI::~UI()
{
    delete uiContext;
}

void UI::destroy()
{
    delete this;
}

void UI::destroyAllChildren()
{
    root->destroyAllChildren();
}

void UI::process(float delta)
{
    Window *window = uiContext->getWindow();
    root->width.setAsNumber(window->getWidth());
    root->height.setAsNumber(window->getHeight());
    root->positioning.set(UIBlockPositioning::Absolute);
    root->process(delta);
}

void UI::render()
{
    Window *window = uiContext->getWindow();
    Renderer *renderer = uiContext->getRenderer();

    // printf("\nRender UI start\n");
    uiContext->cleanForNewRender();

    root->collectRenderBlock(uiContext);

    uiContext->sortBlocks();

    Matrix4 projectionMatrix = glm::ortho(0.0f, (float)window->getWidth(), (float)window->getHeight(), 0.0f, -1.0f, 1.0f);
    Matrix4 viewMatrix = Matrix4(1.0f);

    renderer->setupSpriteRendering(viewMatrix, projectionMatrix);
    int blocksCount = uiContext->getBlocksCount();

    Entity entity;
    for (int i = 0; i < blocksCount; i++)
    {
        UIRenderBlock &block = uiContext->getBlock(i);
        UINode *node = block.source;
        if (!node)
            continue;

        if (node->getCalculatedColorBackground().a != 0.0f)
        {
            entity.setPosition(block.x + node->getCalculatedMarginLeft(), block.y + node->getCalculatedMarginTop(), 0.0f);
            entity.setScale(node->getCalculatedFilledWidth(), node->getCalculatedFilledHeight());
            renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBackground());
        }
    }
}
