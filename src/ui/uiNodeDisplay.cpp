// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "uiNodeDisplay.h"

bool UINodeDisplay::insertFrom(UINodeDisplay *nodeDisplay)
{
    bool out = false;
    if (nodeDisplay->width.isSet())
    {
        if (nodeDisplay->width.isUsingNumber())
            width.setAsNumber(nodeDisplay->width.getValue());
        else
            width.setAsPercentage(nodeDisplay->width.getValue());
        out = true;
    }

    if (nodeDisplay->height.isSet())
    {
        if (nodeDisplay->height.isUsingNumber())
            height.setAsNumber(nodeDisplay->height.getValue());
        else
            height.setAsPercentage(nodeDisplay->height.getValue());
        out = true;
    }

    if (nodeDisplay->maxWidth.isSet())
    {
        if (nodeDisplay->maxWidth.isUsingNumber())
            maxWidth.setAsNumber(nodeDisplay->maxWidth.getValue());
        else
            maxWidth.setAsPercentage(nodeDisplay->maxWidth.getValue());
        out = true;
    }

    if (nodeDisplay->maxHeight.isSet())
    {
        if (nodeDisplay->maxHeight.isUsingNumber())
            maxHeight.setAsNumber(nodeDisplay->maxHeight.getValue());
        else
            maxHeight.setAsPercentage(nodeDisplay->maxHeight.getValue());
        out = true;
    }

    if (nodeDisplay->minWidth.isSet())
    {
        if (nodeDisplay->minWidth.isUsingNumber())
            minWidth.setAsNumber(nodeDisplay->minWidth.getValue());
        else
            minWidth.setAsPercentage(nodeDisplay->minWidth.getValue());
        out = true;
    }

    if (nodeDisplay->minHeight.isSet())
    {
        if (nodeDisplay->minHeight.isUsingNumber())
            minHeight.setAsNumber(nodeDisplay->minHeight.getValue());
        else
            minHeight.setAsPercentage(nodeDisplay->minHeight.getValue());
        out = true;
    }

    for (int i = 0; i < 4; i++)
    {
        if (nodeDisplay->margin[i].isSet())
        {
            if (nodeDisplay->margin[i].isUsingNumber())
                margin[i].setAsNumber(nodeDisplay->margin[i].getValue());
            else
                margin[i].setAsPercentage(nodeDisplay->margin[i].getValue());
            out = true;
        }
        if (nodeDisplay->padding[i].isSet())
        {
            if (nodeDisplay->padding[i].isUsingNumber())
                padding[i].setAsNumber(nodeDisplay->padding[i].getValue());
            else
                padding[i].setAsPercentage(nodeDisplay->padding[i].getValue());
            out = true;
        }
        if (nodeDisplay->border[i].isSet())
        {
            if (nodeDisplay->border[i].isUsingNumber())
                border[i].setAsNumber(nodeDisplay->border[i].getValue());
            else
                border[i].setAsPercentage(nodeDisplay->border[i].getValue());
            out = true;
        }
    }

    if (nodeDisplay->horizontalAlign.isSet())
    {
        horizontalAlign.set(nodeDisplay->horizontalAlign.getValue());
        out = true;
    }
    if (nodeDisplay->verticalAlign.isSet())
    {
        verticalAlign.set(nodeDisplay->verticalAlign.getValue());
        out = true;
    }
    if (nodeDisplay->textHorizontalAlign.isSet())
    {
        textHorizontalAlign.set(nodeDisplay->textHorizontalAlign.getValue());
        out = true;
    }
    if (nodeDisplay->textVerticalAlign.isSet())
    {
        textVerticalAlign.set(nodeDisplay->textVerticalAlign.getValue());
        out = true;
    }
    if (nodeDisplay->contentDirection.isSet())
    {
        contentDirection.set(nodeDisplay->contentDirection.getValue());
        out = true;
    }
    if (nodeDisplay->positioning.isSet())
    {
        positioning.set(nodeDisplay->positioning.getValue());
        out = true;
    }
    if (nodeDisplay->text.isSet())
    {
        text.set(nodeDisplay->text.getValue());
        out = true;
    }
    if (nodeDisplay->wordWrap.isSet())
    {
        wordWrap.set(nodeDisplay->wordWrap.getValue());
    }
    if (nodeDisplay->colorBackground.isSet())
    {
        colorBackground.set(nodeDisplay->colorBackground.getValue());
        out = true;
    }
    for (int i = 0; i < 4; i++)
    {
        if (nodeDisplay->colorBorder[i].isSet())
        {
            colorBorder[i].set(nodeDisplay->colorBorder[i].getValue());
            out = true;
        }
    }
    if (nodeDisplay->colorText.isSet())
    {
        colorText.set(nodeDisplay->colorText.getValue());
        out = true;
    }
    if (nodeDisplay->colorImageMask.isSet())
    {
        colorImageMask.set(nodeDisplay->colorImageMask.getValue());
        out = true;
    }
    if (nodeDisplay->font.isSet())
    {
        font.set(nodeDisplay->font.getValue());
        out = true;
    }
    if (nodeDisplay->fontSize.isSet())
    {
        fontSize.set(nodeDisplay->fontSize.getValue());
        out = true;
    }
    if (nodeDisplay->letterSpacing.isSet())
    {
        letterSpacing.set(nodeDisplay->letterSpacing.getValue());
        out = true;
    }
    if (nodeDisplay->lineSpacing.isSet())
    {
        lineSpacing.set(nodeDisplay->lineSpacing.getValue());
        out = true;
    }
    if (nodeDisplay->image.isSet())
    {
        image.set(nodeDisplay->image.getValue());
        out = true;
    }
    if (nodeDisplay->imageHorizontalAlign.isSet())
    {
        imageHorizontalAlign.set(nodeDisplay->imageHorizontalAlign.getValue());
        out = true;
    }
    if (nodeDisplay->imageVerticalAlign.isSet())
    {
        imageVerticalAlign.set(nodeDisplay->imageVerticalAlign.getValue());
        out = true;
    }
    if (propagateHover.isNotSet() && nodeDisplay->propagateHover.isSet())
    {
        propagateHover.set(nodeDisplay->propagateHover.getValue());
        out = true;
    }
    if (nodeDisplay->visibility.isSet())
    {
        visibility.set(nodeDisplay->visibility.getValue());
        out = true;
    }
    if (nodeDisplay->imageHorizontalAlign.isSet())
    {
        imageHorizontalAlign.set(nodeDisplay->imageHorizontalAlign.getValue());
        out = true;
    }
    if (nodeDisplay->cursorIcon.isSet())
    {
        cursorIcon.set(nodeDisplay->cursorIcon.getValue());
        out = true;
    }
    if (nodeDisplay->overflow.isSet())
    {
        overflow.set(nodeDisplay->overflow.getValue());
        out = true;
    }
    return out;
}

void UINodeDisplay::clear()
{
    width.unSet();
    height.unSet();
    maxWidth.unSet();
    maxHeight.unSet();
    minWidth.unSet();
    minHeight.unSet();
    for (int i = 0; i < 4; i++)
    {
        margin[i].unSet();
        padding[i].unSet();
        border[i].unSet();
    }
    horizontalAlign.unSet();
    verticalAlign.unSet();
    textHorizontalAlign.unSet();
    textVerticalAlign.unSet();
    contentDirection.unSet();
    positioning.unSet();
    text.unSet();
    wordWrap.unSet();
    colorBackground.unSet();
    for (int i = 0; i < 4; i++)
        colorBorder[i].unSet();
    colorText.unSet();
    colorSelection.unSet();
    colorImageMask.unSet();
    font.unSet();
    fontSize.unSet();
    letterSpacing.unSet();
    lineSpacing.unSet();
    image.unSet();
    imageHorizontalAlign.unSet();
    imageVerticalAlign.unSet();
    propagateHover.unSet();
    visibility.unSet();
    cursorIcon.unSet();
    overflow.unSet();
}