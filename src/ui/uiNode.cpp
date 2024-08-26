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

void UINode::processStyle()
{
    style.clear();
    style.insertFrom(this);
    if (bHovered)
        style.insertFrom(&hover);
    if (isVisible())
    {
        rebuild();
        for (auto &node : children)
            node->processStyle();
    }
}

void UINode::process(float delta)
{
    if (isDestroyed())
    {
        for (auto &node : children)
            node->destroy();
    }
    for (auto &node : children)
        node->process(delta);
}

void UINode::removeDestroyed()
{
    for (auto &node : children)
        node->removeDestroyed();

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

void UINode::clearHover()
{
    bHovered = false;
    for (auto &node : children)
        node->clearHover();
}

void UINode::collectRenderBlock(UIContext *uiContext)
{
    if (!isVisible())
        return;

    float xStartPosition = uiContext->xPosition;
    float yStartPosition = uiContext->yPosition;
    int index = (calculatedPositioning == UIBlockPositioning::Absolute) ? uiContext->index + 100 : uiContext->index + 1;

    UIRenderBlock *attachTo = nullptr;
    if (calculatedPositioning == UIBlockPositioning::Inline || calculatedPositioning == UIBlockPositioning::Relative)
        attachTo = uiContext->parentBlock ? uiContext->parentBlock : uiContext->initialBlock;
    else
    {
        attachTo = uiContext->rootBlock;
        xStartPosition = attachTo->x;
        yStartPosition = attachTo->y;
    }

    bool createBlock = hasDisplayableData() || (calculatedPositioning != UIBlockPositioning::Inline);

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

        renderBlock->imageShiftX = 0.0f;
        renderBlock->imageShiftY = 0.0f;
        if (calculatedImageHorizontalAlign == UIContentAlign::Middle)
            renderBlock->imageShiftX = (calculatedWidth - getImageWidth()) * 0.5f;
        if (calculatedImageHorizontalAlign == UIContentAlign::End)
            renderBlock->imageShiftX = (calculatedWidth - getImageWidth());
        if (calculatedImageVerticalAlign == UIContentAlign::Middle)
            renderBlock->imageShiftY = (calculatedHeight - getImageHeight()) * 0.5f;
        if (calculatedImageVerticalAlign == UIContentAlign::End)
            renderBlock->imageShiftY = (calculatedHeight - getImageHeight());

        renderBlock->hoverWidth = calculatedWidth + calculatedPadding[UI_LEFT] + calculatedPadding[UI_RIGHT] + calculatedBorder[UI_LEFT] + calculatedBorder[UI_RIGHT];
        renderBlock->hoverHeight = calculatedHeight + calculatedPadding[UI_TOP] + calculatedPadding[UI_BOTTOM] + calculatedBorder[UI_TOP] + calculatedBorder[UI_BOTTOM];
        renderBlock->hoverShiftX = getCalculatedMarginLeft();
        renderBlock->hoverShiftY = getCalculatedMarginTop();

        renderBlock->source = this;

        if (calculatedPositioning == UIBlockPositioning::Absolute || calculatedPositioning == UIBlockPositioning::Relative)
            uiContext->rootBlock = renderBlock;
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
    if (!isVisible())
        return false;
    if (calculatedWidth != 0.0f && calculatedHeight != 0.0f && children.size() > 0)
        return true;
    if (calculatedColorBackground.a != 0.0f)
        return true;
    for (int i = 0; i < 4; i++)
    {
        if (calculatedColorBorder[i].a > 0.0f && calculatedBorder[i] > 0.0f)
            return true;
    }
    if (text.isSet() && text.getValue().size() > 0)
        return true;
    if (image.isSet() && image.getValue())
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

float UINode::getImageWidth()
{
    if (image.isSet() && image.getValue())
        return image.getValue()->getWidth();
    return 0.0f;
}

float UINode::getImageHeight()
{
    if (image.isSet() && image.getValue())
        return image.getValue()->getHeight();
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
    if (style.width.isSet())
    {
        if (style.width.isUsingPercentage())
            assumedWidth = (style.width.getValue() * 0.01f) * getParentFreeWidth() - getMarginPaddingBorderWidth();
        else
            assumedWidth = style.width.getValue();
    }
    if (style.maxWidth.isSet())
    {
        float maxWidthFinal = style.maxWidth.isUsingPercentage() ? style.maxWidth.getValue() * 0.01f * getParentFreeWidth() : style.maxWidth.getValue();
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
    if (style.font.isSet())
        return style.font.getValue();
    if (parent)
        return parent->getFont();
    return nullptr;
}

unsigned int UINode::getFontHeight()
{
    if (style.fontSize.isSet())
        return style.fontSize.getValue();
    if (parent)
        return parent->getFontHeight();
    return 24;
}

Color UINode::getColorText()
{
    if (style.colorText.isSet())
        return style.colorText.getValue();
    if (parent)
        return parent->getColorText();
    return Color(0, 0, 0, 1);
}

Color UINode::getColorSelection()
{
    if (style.colorSelection.isSet())
        return style.colorSelection.getValue();
    if (parent)
        return parent->getColorSelection();
    return Color(0, 0, 0, 1);
}

float UINode::getLetterSpacing()
{
    if (style.letterSpacing.isSet())
        return style.letterSpacing.getValue();
    if (parent)
        return parent->getLetterSpacing();
    return 0.0f;
}

float UINode::getLineSpacing()
{
    if (style.lineSpacing.isSet())
        return style.lineSpacing.getValue();
    if (parent)
        return parent->getLineSpacing();
    return 0.0f;
}

void UINode::propagateHoverToChildren()
{
    bHovered = true;
    for (auto &node : children)
    {
        if (node->getStyleFinal()->positioning.isNotSet() || node->getStyleFinal()->positioning.getValue() != UIBlockPositioning::Absolute)
            node->propagateHoverToChildren();
    }
}

void UINode::propagateHoverToParents()
{
    bHovered = true;
    if (style.propagateHover.isSet() && style.propagateHover.getValue())
        propagateHoverToChildren();
    if (parent && (style.positioning.isNotSet() || style.positioning.getValue() != UIBlockPositioning::Absolute))
        parent->propagateHoverToParents();
}

MouseCursorIcon UINode::getNearestCursorIcon()
{
    if (style.cursorIcon.isSet())
        return style.cursorIcon.getValue();
    if (parent)
        return parent->getNearestCursorIcon();
    return MouseCursorIcon::None;
}

void UINode::gatherNotHovered(std::vector<UINode *> *notHoveredList)
{
    if (bHovered == false && triggersEventClickOutside && isVisible())
    {
        notHoveredList->push_back(this);
        for (auto &child : children)
            child->gatherNotHovered(notHoveredList);
    }
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
    if (image.isSet() && image.getValue())
    {
        width = fmaxf(width, static_cast<float>(image.getValue()->getWidth()));
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
    if (image.isSet() && image.getValue())
    {
        height = fmaxf(height, static_cast<float>(image.getValue()->getHeight()));
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
        if (style.margin[i].isSet())
        {
            calculatedMargin[i] = style.margin[i].isUsingPercentage()
                                      ? ((i == UI_LEFT || i == UI_RIGHT) ? parentFreeWidth : parentFreeHeight) * 0.01f * style.margin[i].getValue()
                                      : style.margin[i].getValue();
        }
        calculatedPadding[i] = 0.0f;
        if (style.padding[i].isSet())
        {
            calculatedPadding[i] = style.padding[i].isUsingPercentage()
                                       ? ((i == UI_LEFT || i == UI_RIGHT) ? parentFreeWidth : parentFreeHeight) * 0.01f * style.padding[i].getValue()
                                       : style.padding[i].getValue();
        }
        calculatedBorder[i] = 0.0f;
        if (style.border[i].isSet())
        {
            calculatedBorder[i] = style.border[i].isUsingPercentage()
                                      ? ((i == UI_LEFT || i == UI_RIGHT) ? parentFreeWidth : parentFreeHeight) * 0.01f * style.border[i].getValue()
                                      : style.border[i].getValue();
        }
    }

    // Width
    if (style.width.isSet())
    {
        if (style.width.isUsingPercentage())
            calculatedWidth = (style.width.getValue() * 0.01f) * parentFreeWidth - getMarginPaddingBorderWidth();
        else
            calculatedWidth = style.width.getValue();
    }
    else
        calculatedWidth = contentWidth;
    if (style.maxWidth.isSet())
    {
        float maxWidthFinal = style.maxWidth.isUsingPercentage() ? style.maxWidth.getValue() * 0.01f * parentFreeWidth : style.maxWidth.getValue();
        calculatedWidth = fminf(calculatedWidth, maxWidthFinal);
    }
    if (style.minWidth.isSet())
    {
        float minWidthFinal = style.minWidth.isUsingPercentage() ? style.minWidth.getValue() * 0.01f * parentFreeWidth : style.minWidth.getValue();
        calculatedWidth = fmaxf(calculatedWidth, minWidthFinal);
    }

    // Height
    if (style.height.isSet())
    {
        if (style.height.isUsingPercentage())
            calculatedHeight = (style.height.getValue() * 0.01f) * parentFreeHeight - getMarginPaddingBorderHeight();
        else
            calculatedHeight = style.height.getValue();
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
    calculatedColorBackground = style.colorBackground.isSet() ? style.colorBackground.getValue() : Color(0, 0, 0, 0);
    calculatedColorBorder[UI_TOP] = style.colorBorder[UI_TOP].isSet() ? style.colorBorder[UI_TOP].getValue() : Color(0, 0, 0, 0);
    calculatedColorBorder[UI_BOTTOM] = style.colorBorder[UI_BOTTOM].isSet() ? style.colorBorder[UI_BOTTOM].getValue() : Color(0, 0, 0, 0);
    calculatedColorBorder[UI_LEFT] = style.colorBorder[UI_LEFT].isSet() ? style.colorBorder[UI_LEFT].getValue() : Color(0, 0, 0, 0);
    calculatedColorBorder[UI_RIGHT] = style.colorBorder[UI_RIGHT].isSet() ? style.colorBorder[UI_RIGHT].getValue() : Color(0, 0, 0, 0);

    // Font / Text
    calculatedFont = getFont();
    calculatedFontSize = getFontHeight();
    calculatedColorText = getColorText();
    calculatedColorSelection = getColorSelection();
    calculatedLetterSpacing = getLetterSpacing();
    calculatedLineSpacing = getLineSpacing();

    calculatedTextHorizontalAlign = style.textHorizontalAlign.isSet() ? style.textHorizontalAlign.getValue() : UIContentAlign::Start;
    if (calculatedTextHorizontalAlign == UIContentAlign::SpaceAround || calculatedTextHorizontalAlign == UIContentAlign::SpaceBetween)
        calculatedTextHorizontalAlign = UIContentAlign::Middle;
    calculatedTextVerticalAlign = style.textVerticalAlign.isSet() ? style.textVerticalAlign.getValue() : UIContentAlign::Start;
    if (calculatedTextVerticalAlign == UIContentAlign::SpaceAround || calculatedTextVerticalAlign == UIContentAlign::SpaceBetween)
        calculatedTextVerticalAlign = UIContentAlign::Middle;

    calculatedImageHorizontalAlign = style.imageHorizontalAlign.isSet() ? style.imageHorizontalAlign.getValue() : UIContentAlign::Start;
    if (calculatedImageHorizontalAlign == UIContentAlign::SpaceAround || calculatedImageHorizontalAlign == UIContentAlign::SpaceBetween)
        calculatedImageHorizontalAlign = UIContentAlign::Middle;
    calculatedImageVerticalAlign = style.imageVerticalAlign.isSet() ? style.imageVerticalAlign.getValue() : UIContentAlign::Start;
    if (calculatedImageVerticalAlign == UIContentAlign::SpaceAround || calculatedImageVerticalAlign == UIContentAlign::SpaceBetween)
        calculatedImageVerticalAlign = UIContentAlign::Middle;

    // Image
    calculatedColorImageMask = style.colorImageMask.isSet() ? style.colorImageMask.getValue() : Color(0, 0, 0, 0);
    calculatedUseImageMask = style.colorImageMask.isSet();
    calculatedImage = style.image.isSet() ? style.image.getValue() : nullptr;
}
