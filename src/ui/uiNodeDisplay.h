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
#include "data/font.h"

class UINodeDisplay
{
public:
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
    UIPropertyDimension margin[4];
    UIPropertyDimension padding[4];
    UIPropertyDimension border[4];
    UIPropertyAlign horizontalAlign;
    UIPropertyAlign verticalAlign;
    UIPropertyDirection contentDirection;
    UIPropertyPositioning positioning;
    UIPropertyColor colorBackground;
    UIPropertyColor colorBorder;
    UIPropertyColor colorText;
    UIPropertyColor colorSelection;
    UIPropertyFont font;
};