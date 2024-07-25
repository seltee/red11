// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "utils/destroyable.h"
#include "uiContext.h"
#include "uiUtils.h"
#include "uiNodeDisplay.h"
#include "uiRenderBlock.h"

// Note - percentage is 100.0f for full, not 1.0f

class UINode : public UINodeDisplay, public Destroyable
{
public:
    EXPORT UINode(UINode *parent);
    EXPORT virtual ~UINode();

    template <class T, typename std::enable_if<std::is_base_of<UINode, T>::value>::type * = nullptr>
    EXPORT T *createUINodeChild()
    {
        auto newNode = new T(this);
        prepareNewNode(newNode);
        return newNode;
    }

    EXPORT void destroyAllChildren();
    EXPORT void process(float delta);
    EXPORT virtual void subProcess(float delta);

    EXPORT void collectRenderBlock(UIContext *uiContext);

    EXPORT bool hasDisplayableData();

    // Calculated - what's calculated
    inline float getCalculatedWidth() { return calculatedWidth; }
    inline float getCalculatedHeight() { return calculatedHeight; }
    inline float getCalculatedMarginLeft() { return calculatedMargin[UI_LEFT]; }
    inline float getCalculatedMarginTop() { return calculatedMargin[UI_TOP]; }
    inline float getCalculatedMarginRight() { return calculatedMargin[UI_RIGHT]; }
    inline float getCalculatedMarginBottom() { return calculatedMargin[UI_BOTTOM]; }
    inline float getCalculatedPaddingLeft() { return calculatedPadding[UI_LEFT]; }
    inline float getCalculatedPaddingTop() { return calculatedPadding[UI_TOP]; }
    inline float getCalculatedPaddingRight() { return calculatedPadding[UI_RIGHT]; }
    inline float getCalculatedPaddingBottom() { return calculatedPadding[UI_BOTTOM]; }
    inline float getCalculatedFullWidth()
    {
        return calculatedWidth + calculatedMargin[UI_LEFT] + calculatedMargin[UI_RIGHT] + calculatedPadding[UI_LEFT] + calculatedPadding[UI_RIGHT] + calculatedMargin[UI_RIGHT] + calculatedBorder[UI_LEFT] + calculatedBorder[UI_RIGHT];
    }
    inline float getCalculatedFullHeight()
    {
        return calculatedHeight + calculatedMargin[UI_TOP] + calculatedMargin[UI_BOTTOM] + calculatedPadding[UI_TOP] + calculatedPadding[UI_BOTTOM] + calculatedBorder[UI_TOP] + calculatedBorder[UI_BOTTOM];
    }
    inline float getCalculatedFilledWidth() { return calculatedWidth + calculatedPadding[UI_LEFT] + calculatedPadding[UI_RIGHT]; }
    inline float getCalculatedFilledHeight() { return calculatedHeight + calculatedPadding[UI_TOP] + calculatedPadding[UI_BOTTOM]; }
    inline Color &getCalculatedColorBackground() { return calculatedColorBackground; }
    inline Color &getCalculatedColorBorder() { return calculatedColorBorder; }
    inline Color &getCalculatedColorText() { return calculatedColorText; }
    inline Color &getCalculatedColorSelection() { return calculatedColorSelection; }
    inline UIContentAlign getCalculatedAlignH() { return calculatedAlignH; }
    inline UIContentAlign getCalculatedAlignV() { return calculatedAlignV; }
    inline UIContentDirection getCalculatedDirection() { return calculatedDirection; }

    // only static content dimensions excluding percentage type content
    EXPORT float getFreeWidth();
    EXPORT float getFreeHeight();

protected:
    EXPORT void prepareNewNode(UINode *node);
    EXPORT virtual float getContentWidth();
    EXPORT virtual float getContentHeight();
    EXPORT virtual float getMarginPaddingBorderWidth();
    EXPORT virtual float getMarginPaddingBorderHeight();
    EXPORT float getParentFreeWidth();
    EXPORT float getParentFreeHeight();
    EXPORT void rebuild();

    // Control data
    UINode *parent = nullptr;
    std::vector<UINode *> children;

    // Calculated data
    float calculatedWidth = 0.0f;
    float calculatedHeight = 0.0f;
    float calculatedMargin[4] = {0, 0, 0, 0};
    float calculatedPadding[4] = {0, 0, 0, 0};
    float calculatedBorder[4] = {0, 0, 0, 0};
    Color calculatedColorBackground = Color(0, 0, 0, 0);
    Color calculatedColorBorder = Color(0, 0, 0, 1);
    Color calculatedColorText = Color(0, 0, 0, 1);
    Color calculatedColorSelection = Color(0.4, 0.4, 0.9f, 1);
    UIContentAlign calculatedAlignH = UIContentAlign::Start;
    UIContentAlign calculatedAlignV = UIContentAlign::Start;
    UIContentDirection calculatedDirection = UIContentDirection::Horizontal;
    UIBlockPositioning calculatedPositioning = UIBlockPositioning::Inline;
};