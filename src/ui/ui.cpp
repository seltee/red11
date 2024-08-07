// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "ui.h"
#include "red11.h"
#include <string>
#include <locale>
#include <codecvt>

UI::UI(Window *window, Renderer *renderer, Font *defaultFont)
{
    uiContext = new UIContext(window, renderer, defaultFont);
    root = new UINode(nullptr);

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
            float posX = block.x + node->getCalculatedMarginLeft() + node->getCalculatedBorderLeft();
            float posY = block.y + node->getCalculatedMarginTop() + node->getCalculatedBorderTop();

            if (node->getCalculatedColorBackground().a != 0.0f)
            {
                entity.setPosition(posX, posY, -0.5f);
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
                float lineSpacing = node->getCalculatedLineSpacing() / interfaceZoom;

                float startPos = posX + node->getCalculatedPaddingLeft() + block.textShiftX;
                float xPosIterator = startPos;
                float yPosIterator = posY + node->getCalculatedPaddingTop() + block.textShiftY;

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
                                -0.5f);
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
                    -0.5f);
                entity.setScale(image->getWidth(), image->getHeight());

                if (useMask)
                    renderer->renderSpriteMask(entity.getModelMatrix(), image, node->getCalculatedColorImageMask());
                else
                    renderer->renderSpriteImage(entity.getModelMatrix(), image);
            }
        }
    }
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
            node = node->getParent();
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
            node = node->getParent();
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

float UI::getNextWordWidth(std::u32string &str, Font *font, unsigned int position, unsigned int fontSize, float letterSpacing)
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