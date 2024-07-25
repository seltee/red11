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
    root->width.setAsNumber(window->getWidth());
    root->height.setAsNumber(window->getHeight());
    root->font.set(uiContext->getDefaultFont());
    root->fontSize.set(24);
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
                Color &textColor = node->getCalculatedColorText();
                float letterSpacing = node->getCalculatedLetterSpacing();

                float xPosIterator = posX + node->getCalculatedPaddingLeft() + block.textShiftX;
                float yPosIterator = posY + node->getCalculatedPaddingTop() + block.textShiftY;

                std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
                std::u32string text32 = convert.from_bytes(text);

                if (font)
                {
                    for (auto &c : text32)
                    {
                        Glyph *glyph = font->getGlyph(c, fontSize);
                        if (glyph && glyph->texture)
                        {
                            float localScale = glyph->texture->getWidth();
                            entity.setPosition(xPosIterator + glyph->shiftX, yPosIterator + glyph->shiftY + fontSize * 4 / 5, 0.0f);
                            entity.setScale(localScale, localScale);
                            renderer->renderSpriteMask(entity.getModelMatrix(), glyph->texture, textColor);
                            xPosIterator += glyph->w + letterSpacing;
                        }
                    }
                }
            }
        }
    }
}
