// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "uiUtils.h"
#include "uiPropertyDimension.h"
#include "uiPropertyAlign.h"
#include "uiPropertyDirection.h"
#include "uiPropertyPositioning.h"
#include "uiPropertyColor.h"
#include "uiPropertyFont.h"
#include "uiPropertyText.h"
#include "uiPropertyNumber.h"
#include "uiPropertyFloat.h"
#include "uiPropertyTexture.h"
#include "uiPropertyToggle.h"
#include "uiPropertyCursorIcon.h"
#include "uiPropertyWordWrap.h"
#include "data/font.h"

class UINodeDisplay
{
public:
    // Returns true if foreign node display changed something
    bool insertFrom(UINodeDisplay *nodeDisplay);
    // Unsets all values
    void clear();

    inline void setMarginNumber(float left, float top, float right, float bottom)
    {
        margin[UI_LEFT].setAsNumber(left);
        margin[UI_TOP].setAsNumber(top);
        margin[UI_RIGHT].setAsNumber(right);
        margin[UI_BOTTOM].setAsNumber(bottom);
    }
    inline void setPaddingNumber(float left, float top, float right, float bottom)
    {
        padding[UI_LEFT].setAsNumber(left);
        padding[UI_TOP].setAsNumber(top);
        padding[UI_RIGHT].setAsNumber(right);
        padding[UI_BOTTOM].setAsNumber(bottom);
    }
    inline void setBorderNumber(float left, float top, float right, float bottom)
    {
        border[UI_LEFT].setAsNumber(left);
        border[UI_TOP].setAsNumber(top);
        border[UI_RIGHT].setAsNumber(right);
        border[UI_BOTTOM].setAsNumber(bottom);
    }
    inline void setMarginNumber(float leftRight, float topBottom)
    {
        margin[UI_LEFT].setAsNumber(leftRight);
        margin[UI_TOP].setAsNumber(topBottom);
        margin[UI_RIGHT].setAsNumber(leftRight);
        margin[UI_BOTTOM].setAsNumber(topBottom);
    }
    inline void setPaddingNumber(float leftRight, float topBottom)
    {
        padding[UI_LEFT].setAsNumber(leftRight);
        padding[UI_TOP].setAsNumber(topBottom);
        padding[UI_RIGHT].setAsNumber(leftRight);
        padding[UI_BOTTOM].setAsNumber(topBottom);
    }
    inline void setBorderNumber(float leftRight, float topBottom)
    {
        border[UI_LEFT].setAsNumber(leftRight);
        border[UI_TOP].setAsNumber(topBottom);
        border[UI_RIGHT].setAsNumber(leftRight);
        border[UI_BOTTOM].setAsNumber(topBottom);
    }
    inline void setMarginNumber(float leftTopRightBottom)
    {
        margin[UI_LEFT].setAsNumber(leftTopRightBottom);
        margin[UI_TOP].setAsNumber(leftTopRightBottom);
        margin[UI_RIGHT].setAsNumber(leftTopRightBottom);
        margin[UI_BOTTOM].setAsNumber(leftTopRightBottom);
    }
    inline void setPaddingNumber(float leftTopRightBottom)
    {
        padding[UI_LEFT].setAsNumber(leftTopRightBottom);
        padding[UI_TOP].setAsNumber(leftTopRightBottom);
        padding[UI_RIGHT].setAsNumber(leftTopRightBottom);
        padding[UI_BOTTOM].setAsNumber(leftTopRightBottom);
    }
    inline void setBorderNumber(float leftTopRightBottom)
    {
        border[UI_LEFT].setAsNumber(leftTopRightBottom);
        border[UI_TOP].setAsNumber(leftTopRightBottom);
        border[UI_RIGHT].setAsNumber(leftTopRightBottom);
        border[UI_BOTTOM].setAsNumber(leftTopRightBottom);
    }
    inline void setColorBorderLeft(Color color)
    {
        colorBorder[UI_LEFT].set(color);
    }
    inline void setColorBorderRight(Color color)
    {
        colorBorder[UI_RIGHT].set(color);
    }
    inline void setColorBorderTop(Color color)
    {
        colorBorder[UI_TOP].set(color);
    }
    inline void setColorBorderBottom(Color color)
    {
        colorBorder[UI_BOTTOM].set(color);
    }
    inline void setBorderColor(Color colorLeft, Color colorTop, Color colorRight, Color colorBottom)
    {
        colorBorder[UI_LEFT].set(colorLeft);
        colorBorder[UI_TOP].set(colorTop);
        colorBorder[UI_RIGHT].set(colorRight);
        colorBorder[UI_BOTTOM].set(colorBottom);
    }

    inline UIPropertyDimension &getMarginLeft() { return margin[UI_LEFT]; }
    inline UIPropertyDimension &getMarginTop() { return margin[UI_TOP]; }
    inline UIPropertyDimension &getMarginRight() { return margin[UI_RIGHT]; }
    inline UIPropertyDimension &getMarginBottom() { return margin[UI_BOTTOM]; }
    inline UIPropertyDimension &getPaddingLeft() { return padding[UI_LEFT]; }
    inline UIPropertyDimension &getPaddingTop() { return padding[UI_TOP]; }
    inline UIPropertyDimension &getPaddingRight() { return padding[UI_RIGHT]; }
    inline UIPropertyDimension &getPaddingBottom() { return padding[UI_BOTTOM]; }
    inline UIPropertyDimension &getBorderLeft() { return border[UI_LEFT]; }
    inline UIPropertyDimension &getBorderTop() { return border[UI_TOP]; }
    inline UIPropertyDimension &getBorderRight() { return border[UI_RIGHT]; }
    inline UIPropertyDimension &getBorderBottom() { return border[UI_BOTTOM]; }

    UIPropertyDimension width;
    UIPropertyDimension height;
    UIPropertyDimension maxWidth;
    UIPropertyDimension maxHeight;
    UIPropertyDimension minWidth;
    UIPropertyDimension minHeight;
    UIPropertyDimension margin[4];
    UIPropertyDimension padding[4];
    UIPropertyDimension border[4];
    UIPropertyAlign horizontalAlign;
    UIPropertyAlign verticalAlign;
    UIPropertyAlign textHorizontalAlign;
    UIPropertyAlign textVerticalAlign;
    UIPropertyDirection contentDirection;
    UIPropertyPositioning positioning;
    UIPropertyText text;
    UIPropertyWordWrap wordWrap;
    UIPropertyColor colorBackground;
    UIPropertyColor colorBorder[4];
    UIPropertyColor colorText;
    UIPropertyColor colorSelection;
    UIPropertyColor colorImageMask;
    UIPropertyFont font;
    UIPropertyNumber fontSize;
    UIPropertyFloat letterSpacing;
    UIPropertyFloat lineSpacing;
    UIPropertyTexture image;
    UIPropertyAlign imageHorizontalAlign;
    UIPropertyAlign imageVerticalAlign;
    UIPropertyToggle propagateHover;
    UIPropertyToggle visibility;
    UIPropertyCursorIcon cursorIcon;
};