// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "ui.h"
#include <string>
#include <locale>
#include <codecvt>

UI::UI(Window *window, Renderer *renderer, Font *defaultFont)
{
    uiContext = new UIContext(window, renderer, defaultFont);
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
    root->width.setAsNumber(window->getWidth() / interfaceZoom);
    root->height.setAsNumber(window->getHeight() / interfaceZoom);
    root->font.set(uiContext->getDefaultFont());
    root->fontSize.set(24);
    root->positioning.set(UIBlockPositioning::Absolute);

    root->processStyle();
    root->process(delta);
    root->clearHover();

    uiContext->cleanForNewRender();
    root->collectRenderBlock(uiContext);
    uiContext->sortBlocks();

    MousePosition p = window->getMousePosition();
    p.x = static_cast<int>(static_cast<float>(p.x) / interfaceZoom);
    p.y = static_cast<int>(static_cast<float>(p.y) / interfaceZoom);
    int blocksCount = uiContext->getBlocksCount();

    if (window->isCursorOverWindow())
    {
        UIRenderBlock *hoveredBlock = nullptr;
        for (int i = 0; i < blocksCount; i++)
        {
            UIRenderBlock &block = uiContext->getBlock(i);
            float hoverX = block.x + block.hoverShiftX;
            float hoverY = block.y + block.hoverShiftY;

            if (block.source && p.x >= hoverX && p.x < hoverX + block.hoverWidth && p.y >= hoverY && p.y < hoverY + block.hoverHeight)
            {
                if (!hoveredBlock || block.index > hoveredBlock->index)
                    hoveredBlock = &block;
            }
        }

        if (hoveredBlock && hoveredBlock->source)
        {
            hoveredBlock->source->propagateHoverToParents();
            MouseCursorIcon icon = hoveredBlock->source->getNearestCursorIcon();
            window->setCursorIcon(icon != MouseCursorIcon::None ? icon : MouseCursorIcon::Default);
        }

        root->processStyle();
    }
}

void UI::render()
{
    Window *window = uiContext->getWindow();
    Renderer *renderer = uiContext->getRenderer();

    // printf("\nRender UI start\n");
    Matrix4 projectionMatrix = glm::ortho(0.0f, (float)window->getWidth() / interfaceZoom, (float)window->getHeight() / interfaceZoom, 0.0f, -1.0f, 1.0f);
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

        if (node->hasDisplayableData())
        {
            float posX = block.x + node->getCalculatedMarginLeft() + node->getCalculatedBorderLeft();
            float posY = block.y + node->getCalculatedMarginTop() + node->getCalculatedBorderTop();

            if (node->getCalculatedColorBackground().a != 0.0f)
            {
                entity.setPosition(posX, posY, 0.0f);
                entity.setScale(node->getCalculatedFilledWidth(), node->getCalculatedFilledHeight());
                renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBackground());
            }
            if (node->hasCalculatedText())
            {
                std::string &text = node->getCalculatedText();
                Font *font = node->getCalculatedFont();
                unsigned int fontSize = node->getCalcualtedFontSize();
                unsigned int fontSizeGlyph = static_cast<unsigned int>(static_cast<float>(fontSize) * interfaceZoom);
                Color &textColor = node->getCalculatedColorText();
                float letterSpacing = node->getCalculatedLetterSpacing() / interfaceZoom;

                float xPosIterator = posX + node->getCalculatedPaddingLeft() + block.textShiftX;
                float yPosIterator = posY + node->getCalculatedPaddingTop() + block.textShiftY;

                std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
                std::u32string text32 = convert.from_bytes(text);

                if (font)
                {
                    for (auto &c : text32)
                    {
                        Glyph *glyph = font->getGlyph(c, fontSizeGlyph);
                        if (glyph && glyph->texture)
                        {
                            float localScale = static_cast<float>(glyph->texture->getWidth()) / interfaceZoom;
                            entity.setPosition(
                                xPosIterator + static_cast<float>(glyph->shiftX) / interfaceZoom,
                                yPosIterator + static_cast<float>(glyph->shiftY) / interfaceZoom + static_cast<float>(fontSize) * 0.8f,
                                0.0f);
                            entity.setScale(localScale, localScale);
                            renderer->renderSpriteMask(entity.getModelMatrix(), glyph->texture, textColor);
                            xPosIterator += static_cast<float>(glyph->w) / interfaceZoom + letterSpacing;
                        }
                    }
                }
            }
            if (node->hasCalculatedImage())
            {
                Texture *image = node->getCalculatedImage();
                bool useMask = node->isCalculatedImageUsingMask();
                entity.setPosition(posX + node->getCalculatedPaddingLeft() + block.imageShiftX, posY + node->getCalculatedPaddingTop() + block.imageShiftY, 0.0f);
                entity.setScale(image->getWidth(), image->getHeight());

                if (useMask)
                    renderer->renderSpriteMask(entity.getModelMatrix(), image, node->getCalculatedColorImageMask());
                else
                    renderer->renderSpriteImage(entity.getModelMatrix(), image);
            }
        }
    }
}