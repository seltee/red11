// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "ui.h"
#include "red11.h"
#include <string>
#include <locale>
#include <codecvt>

const float fZShift = -0.5f;

UI::UI(Window *window, Renderer *renderer, Font *defaultFont)
{
    uiContext = new UIContext(window, renderer, defaultFont);
    root = new UINode(nullptr, this);

    inputProvider = Red11::getGlobalInputProvider();
    InputDescriptorList mouseClickList;
    mouseClickList.addMouseInput(InputMouseType::LeftButton, 1.0f);
    inputProvider->addInput(mouseClickList, this, [](InputType type, InputData *data, float value, void *userData)
                            { ((UI *)userData)->processMouseClick(value); });
}

UI::~UI()
{
    inputProvider->removeInput(clickInputIndex);
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
    root->width.setAsNumber(static_cast<float>(window->getWidth()) / interfaceZoom);
    root->height.setAsNumber(static_cast<float>(window->getHeight()) / interfaceZoom);

    root->font.set(uiContext->getDefaultFont());
    root->fontSize.set(24);
    root->positioning.set(UIBlockPositioning::Absolute);

    if (clickHappened)
    {
        clickHappened = false;
        triggerClick();
    }
    else if (releaseHappened)
    {
        releaseHappened = false;
        triggerRelease();
    }

    root->processStyle();
    root->process(delta);
    root->removeDestroyed();
    root->clearHover();
    root->processStyle();

    uiContext->cleanForNewRender();
    root->collectRenderBlock(uiContext);
    uiContext->sortBlocks();

    MousePosition p = window->getMousePosition();
    p.x = static_cast<int>(static_cast<float>(p.x) / interfaceZoom);
    p.y = static_cast<int>(static_cast<float>(p.y) / interfaceZoom);
    int blocksCount = uiContext->getBlocksCount();

    if (window->isCursorOverWindow())
    {
        hoveredBlock = nullptr;
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

    Matrix4 projectionMatrix = glm::ortho(
        0.0f,
        static_cast<float>(window->getWidth()) / interfaceZoom,
        static_cast<float>(window->getHeight()) / interfaceZoom,
        0.0f, 0.0f, 1.0f);
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
            // cut area
            block.overflowWindow.startH = static_cast<int>(ceilf(static_cast<float>(block.overflowWindow.startH) * interfaceZoom));
            block.overflowWindow.startV = static_cast<int>(ceilf(static_cast<float>(block.overflowWindow.startV) * interfaceZoom));
            block.overflowWindow.endH = static_cast<int>(ceilf(static_cast<float>(block.overflowWindow.endH) * interfaceZoom) + 0.5f);
            block.overflowWindow.endV = static_cast<int>(ceilf(static_cast<float>(block.overflowWindow.endV) * interfaceZoom) + 0.5f);
            renderer->setRenderArea(block.overflowWindow);

            float posX = block.x + node->getCalculatedMarginLeft();
            float posY = block.y + node->getCalculatedMarginTop();
            float posXWithBorder = posX + node->getCalculatedBorderLeft();
            float posYWithBorder = posY + node->getCalculatedBorderTop();
            float borderWidth = node->getCalculatedBorderLeft() + node->getCalculatedBorderRight();
            float borderHeight = node->getCalculatedBorderTop() + node->getCalculatedBorderBottom();

            // Background
            if (node->getCalculatedColorBackground().a != 0.0f)
            {
                entity.setPosition(posXWithBorder, posYWithBorder, fZShift);
                entity.setScale(node->getCalculatedFilledWidth(), node->getCalculatedFilledHeight());
                renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBackground());
            }

            // Borders
            // Left
            if (node->getCalculatedBorderLeft() > 0.0f)
            {
                entity.setPosition(posX, posY, fZShift);
                entity.setScale(node->getCalculatedBorderLeft(), node->getCalculatedFilledHeight() + borderHeight);
                renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBorder(UI_LEFT));
            }

            // Right
            if (node->getCalculatedBorderLeft() > 0.0f)
            {
                entity.setPosition(posXWithBorder + node->getCalculatedFilledWidth(), posY, fZShift);
                entity.setScale(node->getCalculatedBorderLeft(), node->getCalculatedFilledHeight() + borderHeight);
                renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBorder(UI_RIGHT));
            }

            // Top
            if (node->getCalculatedBorderTop() > 0.0f)
            {
                entity.setPosition(posX, posY, fZShift);
                entity.setScale(node->getCalculatedFilledWidth() + borderWidth, node->getCalculatedBorderTop());
                renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBorder(UI_TOP));
            }

            // Bottom
            if (node->getCalculatedBorderBottom() > 0.0f)
            {
                entity.setPosition(posX, posYWithBorder + node->getCalculatedFilledHeight(), fZShift);
                entity.setScale(node->getCalculatedFilledWidth() + borderWidth, node->getCalculatedBorderTop());
                renderer->renderSpriteRect(entity.getModelMatrix(), node->getCalculatedColorBorder(UI_BOTTOM));
            }

            if (node->hasCalculatedText())
            {
                const std::string &text = node->getCalculatedText();
                Font *font = node->calcFont();
                unsigned int fontSize = node->calcFontSize();
                unsigned int fontSizeGlyph = static_cast<unsigned int>(static_cast<float>(fontSize) * interfaceZoom);
                Color &textColor = node->getCalculatedColorText();
                float letterSpacing = node->calcLetterSpacing() / interfaceZoom;
                float lineSpacing = node->calcLineSpacing() / interfaceZoom;

                float startPos = posXWithBorder + node->getCalculatedPaddingLeft() + block.textShiftX;
                float xPosIterator = startPos;
                float yPosIterator = posYWithBorder + node->getCalculatedPaddingTop() + block.textShiftY;

                std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
                std::u32string text32 = convert.from_bytes(text);

                if (font)
                {
                    UIWordWrap wordWrap = node->getStyleFinal()->wordWrap.getValue();
                    float panelWidth = node->getCalculatedWidth();
                    bool newWordStarts = true;

                    for (int i = 0; i < text32.size(); i++)
                    {
                        char32_t c = text32[i];

                        Glyph *glyph = font->getGlyph(c, fontSizeGlyph);

                        if (glyph && glyph->texture)
                        {
                            float glyphWidth = static_cast<float>(glyph->w) / interfaceZoom;
                            if (wordWrap == UIWordWrap::ByLetter && c != 0x20)
                            {
                                if (xPosIterator - startPos + glyphWidth + letterSpacing >= panelWidth)
                                {
                                    xPosIterator = startPos;
                                    yPosIterator += static_cast<float>(fontSizeGlyph) + lineSpacing;
                                }
                            }
                            if (wordWrap == UIWordWrap::ByWord && newWordStarts && startPos != xPosIterator)
                            {
                                float wordWidth = getNextWordWidth(text32, font, i, fontSizeGlyph, letterSpacing);
                                if (xPosIterator - startPos + wordWidth >= panelWidth)
                                {
                                    xPosIterator = startPos;
                                    yPosIterator += static_cast<float>(fontSizeGlyph) + lineSpacing;
                                }
                            }

                            float localScale = roundf(static_cast<float>(glyph->texture->getWidth()) / interfaceZoom);
                            entity.setPosition(
                                roundf(xPosIterator + static_cast<float>(glyph->shiftX) / interfaceZoom),
                                roundf(yPosIterator + static_cast<float>(glyph->shiftY) / interfaceZoom + static_cast<float>(fontSize) * 0.8f),
                                fZShift);
                            entity.setScale(localScale, localScale);
                            renderer->renderSpriteMask(entity.getModelMatrix(), glyph->texture, textColor);
                            xPosIterator += glyphWidth + letterSpacing;
                        }

                        newWordStarts = (c == 0x20);
                    }
                }
            }
            if (node->hasCalculatedImage())
            {
                Texture *image = node->getCalculatedImage();
                bool useMask = node->isCalculatedImageUsingMask();
                entity.setPosition(
                    posX + node->getCalculatedPaddingLeft() + block.imageShiftX,
                    posY + node->getCalculatedPaddingTop() + block.imageShiftY,
                    fZShift);
                entity.setScale(image->getWidth(), image->getHeight());

                if (useMask)
                    renderer->renderSpriteMask(entity.getModelMatrix(), image, node->getCalculatedColorImageMask());
                else
                    renderer->renderSpriteImage(entity.getModelMatrix(), image);
            }
        }
    }
    renderer->setRenderAreaFull();
    renderer->endSpriteRendering();
}

void UI::processMouseClick(float value)
{
    if (value > 0.5f)
        clickHappened = true;
    else
        releaseHappened = true;
}

void UI::triggerClick()
{
    // Click
    if (hoveredBlock)
    {
        UINode *node = hoveredBlock->source;
        while (node)
        {
            if (node->triggersEventClick)
            {
                triggerEvent(UIEvent::Click, node);
            }
            if (node->positioning.isNotSet() || node->positioning.getValue() != UIBlockPositioning::Absolute)
                node = node->getParent();
            else
                node = nullptr;
        }
    }

    // Click outside
    std::vector<UINode *> notHoveredList;
    root->gatherNotHovered(&notHoveredList);
    for (auto &node : notHoveredList)
        triggerEvent(UIEvent::ClickOutside, node);
}

void UI::triggerRelease()
{
    // Release
    if (hoveredBlock)
    {
        UINode *node = hoveredBlock->source;
        while (node)
        {
            if (node->triggersEventRelease)
            {
                triggerEvent(UIEvent::Release, node);
            }
            if (node->positioning.isNotSet() || node->positioning.getValue() != UIBlockPositioning::Absolute)
                node = node->getParent();
            else
                node = nullptr;
        }
    }

    // Release outside
    std::vector<UINode *> notHoveredList;
    root->gatherNotHovered(&notHoveredList);
    for (auto &node : notHoveredList)
        triggerEvent(UIEvent::ReleaseOutside, node);
}

void UI::triggerEvent(UIEvent ev, UINode *node)
{
    for (auto &controller : eventControllers)
    {
        controller->processEvent(ev, node);
    }
}

void UI::prepareNewEventController(UIEventController *eventController)
{
    this->eventControllers.push_back(eventController);
}

float UI::getNextWordWidth(const std::u32string &str, Font *font, unsigned int position, unsigned int fontSize, float letterSpacing)
{
    float size = 0.0f;
    while (str[position] && str[position] != 0x20)
    {
        Glyph *glyph = font->getGlyph(str[position], fontSize);
        size += static_cast<float>(glyph->w) / interfaceZoom;
        position++;
    }
    return size;
}