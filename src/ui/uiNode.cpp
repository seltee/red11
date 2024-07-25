// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "uiNode.h"

UINode::UINode(UINode *parent)
{
    this->parent = parent;
}

UINode::~UINode()
{
}

void UINode::destroyAllChildren()
{
    for (auto &node : children)
        node->destroy();
}

void UINode::process(float delta)
{
    rebuild();
    subProcess(delta);
    rebuild();

    if (isDestroyed())
    {
        for (auto &node : children)
            node->destroy();
    }
    for (auto &node : children)
        node->process(delta);

    auto it = children.begin();
    while (it != children.end())
    {
        if ((*it)->isDestroyed())
        {
            delete (*it);
            it = children.erase(it);
        }
        else
            ++it;
    }
}

void UINode::subProcess(float delta)
{
}

void UINode::collectRenderBlock(UIContext *uiContext)
{
    float xStartPosition = uiContext->xPosition;
    float yStartPosition = uiContext->yPosition;
    int index = (calculatedPositioning == UIBlockPositioning::Absolute) ? uiContext->index + 100 : uiContext->index + 1;

    // printf("Block %i\n", index);
    // printf("size %f %f margin %f %f %f %f\n", calculatedWidth, calculatedHeight, calculatedMargin[0], calculatedMargin[1], calculatedMargin[2], calculatedMargin[3]);
    // printf("%f %f\n", getFreeWidth(), getFreeHeight());

    UIRenderBlock *attachTo = nullptr;
    if (calculatedPositioning == UIBlockPositioning::Inline || calculatedPositioning == UIBlockPositioning::Relative)
        attachTo = uiContext->parentBlock ? uiContext->parentBlock : uiContext->initialBlock;
    else
    {
        attachTo = uiContext->rootBlock;
        xStartPosition = attachTo->x;
        yStartPosition = attachTo->y;
    }

    bool createBlock = hasDisplayableData() || calculatedPositioning != UIBlockPositioning::Inline;

    UIRenderBlock *renderBlock = nullptr;
    if (createBlock)
    {
        renderBlock = uiContext->getBlock();
        attachTo->children.push_back(renderBlock);
        attachTo = renderBlock;
        renderBlock->index = index;
        renderBlock->x = xStartPosition;
        renderBlock->y = yStartPosition;
        renderBlock->textShiftX = 0.0f;
        renderBlock->textShiftY = 0.0f;
        if (calculatedTextHorizontalAlign == UIContentAlign::Middle)
            renderBlock->textShiftX = (calculatedWidth - getTextWidth()) * 0.5f;
        if (calculatedTextHorizontalAlign == UIContentAlign::End)
            renderBlock->textShiftX = (calculatedWidth - getTextWidth());
        if (calculatedTextVerticalAlign == UIContentAlign::Middle)
            renderBlock->textShiftY = (calculatedHeight - getTextHeight()) * 0.5f;
        if (calculatedTextVerticalAlign == UIContentAlign::End)
            renderBlock->textShiftY = (calculatedHeight - getTextHeight());
        renderBlock->source = this;
    }

    float childrenWidth = 0.0f;
    float childrenHeight = 0.0f;
    for (auto &node : children)
    {
        if (calculatedDirection == UIContentDirection::Horizontal)
        {
            childrenWidth += node->getCalculatedFullWidth();
            childrenHeight = fmaxf(childrenHeight, node->getCalculatedFullHeight());
        }
        else
        {
            childrenWidth = fmaxf(childrenWidth, node->getCalculatedFullWidth());
            childrenHeight += node->getCalculatedFullHeight();
        }
    }

    UIContentAlign alignH = calculatedAlignH;
    UIContentAlign alignV = calculatedAlignV;
    if ((alignH == UIContentAlign::SpaceBetween || alignH == UIContentAlign::SpaceAround) && calculatedDirection == UIContentDirection::Vertical)
        alignH = UIContentAlign::Middle;
    if ((alignV == UIContentAlign::SpaceBetween || alignV == UIContentAlign::SpaceAround) && calculatedDirection == UIContentDirection::Horizontal)
        alignV = UIContentAlign::Middle;

    float initialX = 0.0f;
    float initialY = 0.0f;
    float betweenX = 0.0f;
    float betweenY = 0.0f;
    if (alignH == UIContentAlign::End)
        initialX = calculatedWidth - childrenWidth;
    if (alignV == UIContentAlign::End)
        initialY = calculatedHeight - childrenHeight;
    if (alignH == UIContentAlign::Middle)
        initialX = (calculatedWidth - childrenWidth) * 0.5f;
    if (alignV == UIContentAlign::Middle)
        initialY = (calculatedHeight - childrenHeight) * 0.5f;
    if (alignH == UIContentAlign::SpaceBetween)
        betweenX = (calculatedWidth - childrenWidth) / static_cast<float>(children.size() - 1);
    if (alignV == UIContentAlign::SpaceBetween)
        betweenY = (calculatedHeight - childrenHeight) / static_cast<float>(children.size() - 1);
    if (alignH == UIContentAlign::SpaceAround)
    {
        betweenX = (calculatedWidth - childrenWidth) / static_cast<float>(children.size() + 1);
        initialX = betweenX;
    }
    if (alignV == UIContentAlign::SpaceAround)
    {
        betweenY = (calculatedHeight - childrenHeight) / static_cast<float>(children.size() + 1);
        initialY = betweenY;
    }

    // printf("CH %f %f\n", childrenWidth, childrenHeight);

    float posX = xStartPosition + initialX + calculatedMargin[UI_LEFT] + calculatedPadding[UI_LEFT] + calculatedBorder[UI_LEFT];
    float posY = yStartPosition + initialY + calculatedMargin[UI_TOP] + calculatedPadding[UI_TOP] + calculatedBorder[UI_TOP];

    for (auto &node : children)
    {
        float childX = posX;
        float childY = posY;
        if (alignH == UIContentAlign::Middle && calculatedDirection == UIContentDirection::Vertical)
            childX += (childrenWidth - node->getCalculatedFullWidth()) * 0.5f;
        if (alignV == UIContentAlign::Middle && calculatedDirection == UIContentDirection::Horizontal)
            childY += (childrenHeight - node->getCalculatedFullHeight()) * 0.5f;

        uiContext->setParentData(childX, childY, attachTo, index);
        node->collectRenderBlock(uiContext);

        if (node->positioning.getValue() != UIBlockPositioning::Absolute)
        {
            if (calculatedDirection == UIContentDirection::Horizontal)
                posX += node->getCalculatedFullWidth() + betweenX;
            else
                posY += node->getCalculatedFullHeight() + betweenY;
        }
    }
}

bool UINode::hasDisplayableData()
{
    if (calculatedColorBackground.a != 0.0f || calculatedColorBorder.a != 0.0f)
        return true;
    if (text.isSet() && text.getValue().size() > 0)
        return true;
    return false;
}

float UINode::getTextWidth()
{
    if (text.isSet())
    {
        Font *font = getFont();
        float letterSpacingValue = getLetterSpacing();
        return font->measureWidth(text.getValue(), getFontHeight()) + letterSpacingValue * (text.getValue().size() - 1);
    }
    return 0.0f;
}

float UINode::getTextHeight()
{
    if (text.isSet())
        return getFontHeight();
    return 0.0f;
}

float UINode::getFreeWidth()
{
    float occupied = 0.0f;
    if (contentDirection.getValue() == UIContentDirection::Horizontal)
    {
        for (auto &child : children)
        {
            if (!child->width.isUsingPercentage())
            {
                // todo deal if maxWidth is using percantage
                occupied += child->maxWidth.isSet() ? fminf(child->width.getValue(), child->maxWidth.getValue()) : child->width.getValue();
                if (child->getMarginLeft().isUsingNumber())
                    occupied += child->getMarginLeft().getValue();
                if (child->getMarginRight().isUsingNumber())
                    occupied += child->getMarginRight().getValue();
                if (child->getPaddingLeft().isUsingNumber())
                    occupied += child->getPaddingLeft().getValue();
                if (child->getPaddingRight().isUsingNumber())
                    occupied += child->getPaddingRight().getValue();
                if (child->getBorderLeft().isUsingNumber())
                    occupied += child->getBorderLeft().getValue();
                if (child->getBorderRight().isUsingNumber())
                    occupied += child->getBorderRight().getValue();
            }
        }
    }

    float assumedWidth = 0.0f;
    if (width.isSet())
    {
        if (width.isUsingPercentage())
            assumedWidth = (width.getValue() * 0.01f) * getParentFreeWidth() - getMarginPaddingBorderWidth();
        else
            assumedWidth = width.getValue();
    }
    if (maxWidth.isSet())
    {
        float maxWidthFinal = maxWidth.isUsingPercentage() ? maxWidth.getValue() * 0.01f * getParentFreeWidth() : maxWidth.getValue();
        assumedWidth = fminf(assumedWidth, maxWidthFinal);
    }
    return fmaxf(assumedWidth - occupied, 0.0f);
}

float UINode::getFreeHeight()
{
    float occupied = 0.0f;
    if (contentDirection.getValue() == UIContentDirection::Vertical)
    {
        for (auto &child : children)
        {
            if (!child->height.isUsingPercentage())
            {
                // todo deal if maxWidth is using percantage
                occupied += child->maxHeight.isSet() ? fminf(child->height.getValue(), child->maxHeight.getValue()) : child->height.getValue();
                if (child->getMarginTop().isUsingNumber())
                    occupied += child->getMarginTop().getValue();
                if (child->getMarginBottom().isUsingNumber())
                    occupied += child->getMarginBottom().getValue();
                if (child->getPaddingTop().isUsingNumber())
                    occupied += child->getPaddingTop().getValue();
                if (child->getPaddingBottom().isUsingNumber())
                    occupied += child->getPaddingBottom().getValue();
                if (child->getBorderTop().isUsingNumber())
                    occupied += child->getBorderTop().getValue();
                if (child->getBorderBottom().isUsingNumber())
                    occupied += child->getBorderBottom().getValue();
            }
        }
    }

    float assumedHeight = 0.0f;
    if (height.isSet())
    {
        if (height.isUsingPercentage())
            assumedHeight = (height.getValue() * 0.01f) * getParentFreeHeight() - getMarginPaddingBorderHeight();
        else
            assumedHeight = height.getValue();
    }
    if (maxHeight.isSet())
    {
        float maxHeightFinal = maxHeight.isUsingPercentage() ? maxHeight.getValue() * 0.01f * getParentFreeHeight() : maxHeight.getValue();
        assumedHeight = fminf(assumedHeight, maxHeightFinal);
    }
    return fmaxf(assumedHeight - occupied, 0.0f);
}

Font *UINode::getFont()
{
    if (font.isSet())
        return font.getValue();
    if (parent)
        return parent->getFont();
    return nullptr;
}

unsigned int UINode::getFontHeight()
{
    if (fontSize.isSet())
        return fontSize.getValue();
    if (parent)
        return parent->getFontHeight();
    return 24;
}

Color UINode::getColorText()
{
    if (colorText.isSet())
        return colorText.getValue();
    if (parent)
        return parent->getColorText();
    return Color(0, 0, 0, 1);
}

Color UINode::getColorSelection()
{
    if (colorSelection.isSet())
        return colorSelection.getValue();
    if (parent)
        return parent->getColorSelection();
    return Color(0, 0, 0, 1);
}

float UINode::getLetterSpacing()
{
    if (letterSpacing.isSet())
        return letterSpacing.getValue();
    if (parent)
        return parent->getLetterSpacing();
    return 0.0f;
}

void UINode::prepareNewNode(UINode *node)
{
    children.push_back(node);
}

float UINode::getContentWidth()
{
    float width = 0.0f;
    if (contentDirection.getValue() == UIContentDirection::Horizontal)
    {
        for (auto &child : children)
            width += child->getCalculatedFullWidth();
    }
    else
    {
        for (auto &child : children)
            width = fmaxf(width, child->getCalculatedFullWidth());
    }
    if (text.isSet())
    {
        Font *font = getFont();
        if (font)
        {
            float letterSpacingValue = getLetterSpacing();
            width = fmaxf(width, static_cast<float>(font->measureWidth(text.getValue(), getFontHeight())) + letterSpacingValue * (text.getValue().size() - 1));
        }
    }
    return width;
}

float UINode::getContentHeight()
{
    float height = 0.0f;
    if (contentDirection.getValue() == UIContentDirection::Vertical)
    {
        for (auto &child : children)
            height += child->getCalculatedFullHeight();
    }
    else
    {
        for (auto &child : children)
            height = fmaxf(height, child->getCalculatedFullHeight());
    }
    if (text.isSet())
    {
        Font *font = getFont();
        if (font)
            height = fmaxf(height, static_cast<float>(font->measureHeight(text.getValue(), getFontHeight())));
    }
    return height;
}

float UINode::getMarginPaddingBorderWidth()
{
    return calculatedMargin[UI_LEFT] + calculatedMargin[UI_RIGHT] +
           calculatedPadding[UI_LEFT] + calculatedPadding[UI_RIGHT] +
           calculatedBorder[UI_BOTTOM] + calculatedBorder[UI_RIGHT];
}

float UINode::getMarginPaddingBorderHeight()
{
    return calculatedMargin[UI_TOP] + calculatedMargin[UI_BOTTOM] +
           calculatedPadding[UI_TOP] + calculatedPadding[UI_BOTTOM] +
           calculatedBorder[UI_TOP] + calculatedBorder[UI_BOTTOM];
}

float UINode::getParentFreeWidth()
{
    if (parent)
        return parent->getFreeWidth();
    return 0.0f;
}

float UINode::getParentFreeHeight()
{
    if (parent)
        return parent->getFreeHeight();
    return 0.0f;
}

void UINode::rebuild()
{
    float contentWidth = getContentWidth();
    float contentHeight = getContentHeight();
    float parentFreeWidth = getParentFreeWidth();
    float parentFreeHeight = getParentFreeHeight();

    // Calculate basic dimensions
    // Margin, padding and border
    for (int i = 0; i < 4; i++)
    {
        calculatedMargin[i] = 0.0f;
        if (margin[i].isSet())
        {
            calculatedMargin[i] = margin[i].isUsingPercentage()
                                      ? ((i == UI_LEFT || i == UI_RIGHT) ? parentFreeWidth : parentFreeHeight) * 0.01f * margin[i].getValue()
                                      : margin[i].getValue();
        }
        calculatedPadding[i] = 0.0f;
        if (padding[i].isSet())
        {
            calculatedPadding[i] = padding[i].isUsingPercentage()
                                       ? ((i == UI_LEFT || i == UI_RIGHT) ? parentFreeWidth : parentFreeHeight) * 0.01f * padding[i].getValue()
                                       : padding[i].getValue();
        }
        calculatedBorder[i] = 0.0f;
        if (border[i].isSet())
        {
            calculatedBorder[i] = border[i].isUsingPercentage()
                                      ? ((i == UI_LEFT || i == UI_RIGHT) ? parentFreeWidth : parentFreeHeight) * 0.01f * border[i].getValue()
                                      : border[i].getValue();
        }
    }

    // Width
    if (width.isSet())
    {
        if (width.isUsingPercentage())
            calculatedWidth = (width.getValue() * 0.01f) * parentFreeWidth - getMarginPaddingBorderWidth();
        else
            calculatedWidth = width.getValue();
    }
    else
        calculatedWidth = contentWidth;
    if (maxWidth.isSet())
    {
        float maxWidthFinal = maxWidth.isUsingPercentage() ? maxWidth.getValue() * 0.01f * parentFreeWidth : maxWidth.getValue();
        calculatedWidth = fminf(calculatedWidth, maxWidthFinal);
    }
    if (minWidth.isSet())
    {
        float minWidthFinal = minWidth.isUsingPercentage() ? minWidth.getValue() * 0.01f * parentFreeWidth : minWidth.getValue();
        calculatedWidth = fmaxf(calculatedWidth, minWidthFinal);
    }

    // Height
    if (height.isSet())
    {
        if (height.isUsingPercentage())
            calculatedHeight = (height.getValue() * 0.01f) * parentFreeHeight - getMarginPaddingBorderHeight();
        else
            calculatedHeight = height.getValue();
    }
    else
        calculatedHeight = contentHeight;
    if (maxHeight.isSet())
    {
        float maxHeightFinal = maxHeight.isUsingPercentage() ? maxHeight.getValue() * 0.01f * parentFreeHeight : maxHeight.getValue();
        calculatedHeight = fminf(calculatedHeight, maxHeightFinal);
    }
    if (minHeight.isSet())
    {
        float minHeightFinal = minHeight.isUsingPercentage() ? minHeight.getValue() * 0.01f * parentFreeHeight : minHeight.getValue();
        calculatedHeight = fmaxf(calculatedHeight, minHeightFinal);
    }

    // Properties
    calculatedAlignH = horizontalAlign.getValue();
    calculatedAlignV = verticalAlign.getValue();
    calculatedDirection = contentDirection.getValue();
    calculatedPositioning = positioning.getValue();

    // Element View
    calculatedColorBackground = colorBackground.isSet() ? colorBackground.getValue() : Color(0, 0, 0, 0);
    calculatedColorBorder = colorBorder.isSet() ? colorBorder.getValue() : Color(0, 0, 0, 0);

    // Font / Text
    calculatedFont = getFont();
    calculatedFontSize = getFontHeight();
    calculatedColorText = getColorText();
    calculatedColorSelection = getColorSelection();
    calculatedLetterSpacing = getLetterSpacing();
    calculatedTextHorizontalAlign = textHorizontalAlign.isSet() ? textHorizontalAlign.getValue() : UIContentAlign::Start;
    if (calculatedTextHorizontalAlign == UIContentAlign::SpaceAround || calculatedTextHorizontalAlign == UIContentAlign::SpaceBetween)
        calculatedTextHorizontalAlign = UIContentAlign::Middle;
    calculatedTextVerticalAlign = textVerticalAlign.isSet() ? textVerticalAlign.getValue() : UIContentAlign::Start;
    if (calculatedTextVerticalAlign == UIContentAlign::SpaceAround || calculatedTextVerticalAlign == UIContentAlign::SpaceBetween)
        calculatedTextVerticalAlign = UIContentAlign::Middle;
}
