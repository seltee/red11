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
    EXPORT void processStyle();
    EXPORT void process(float delta);
    EXPORT void clearHover();
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
    inline float getCalculatedBorderLeft() { return calculatedBorder[UI_LEFT]; }
    inline float getCalculatedBorderTop() { return calculatedBorder[UI_TOP]; }
    inline float getCalculatedBorderRight() { return calculatedBorder[UI_RIGHT]; }
    inline float getCalculatedBorderBottom() { return calculatedBorder[UI_BOTTOM]; }
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
    inline Color &getCalculatedColorImageMask() { return calculatedColorImageMask; }
    inline UIContentAlign getCalculatedAlignH() { return calculatedAlignH; }
    inline UIContentAlign getCalculatedAlignV() { return calculatedAlignV; }
    inline UIContentDirection getCalculatedDirection() { return calculatedDirection; }
    inline bool hasCalculatedText() { return text.isSet() && text.getValue().size() > 0; }
    inline std::string &getCalculatedText() { return text.getValue(); }
    inline Font *getCalculatedFont() { return calculatedFont; }
    inline unsigned int getCalcualtedFontSize() { return calculatedFontSize; }
    inline float getCalculatedLetterSpacing() { return calculatedLetterSpacing; }
    inline bool hasCalculatedImage() { return image.isSet() && image.getValue() != nullptr; }
    inline Texture *getCalculatedImage() { return calculatedImage; }
    inline bool isCalculatedImageUsingMask() { return calculatedUseImageMask; }

    // based on display data content
    EXPORT virtual float getTextWidth();
    EXPORT virtual float getTextHeight();
    EXPORT virtual float getImageWidth();
    EXPORT virtual float getImageHeight();

    // only static content dimensions excluding percentage type content
    EXPORT float getFreeWidth();
    EXPORT float getFreeHeight();

    // parent propagated properties:
    EXPORT Font *getFont();
    EXPORT unsigned int getFontHeight();
    EXPORT Color getColorText();
    EXPORT Color getColorSelection();
    EXPORT float getLetterSpacing();

    inline UINodeDisplay *getStyleHover() { return &hover; }
    inline UINodeDisplay *getStyleFinal() { return &style; }

    inline bool isHovered() { return bHovered; }
    inline void setHovered() { this->bHovered = true; }
    EXPORT void propagateHoverToChildren();

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
    bool bHovered = false;

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
    Color calculatedColorImageMask = Color(0.4, 0.4, 0.9f, 1);
    UIContentAlign calculatedAlignH = UIContentAlign::Start;
    UIContentAlign calculatedAlignV = UIContentAlign::Start;
    UIContentDirection calculatedDirection = UIContentDirection::Horizontal;
    UIBlockPositioning calculatedPositioning = UIBlockPositioning::Inline;
    Font *calculatedFont = nullptr;
    unsigned int calculatedFontSize = 0;
    float calculatedLetterSpacing = 0.0f;
    UIContentAlign calculatedTextHorizontalAlign = UIContentAlign::Start;
    UIContentAlign calculatedTextVerticalAlign = UIContentAlign::Start;
    bool calculatedUseImageMask = false;
    Texture *calculatedImage = nullptr;
    UIContentAlign calculatedImageHorizontalAlign = UIContentAlign::Start;
    UIContentAlign calculatedImageVerticalAlign = UIContentAlign::Start;

    // style applied if hover
    UINodeDisplay hover;
    // final style that will be calculated and shown
    UINodeDisplay style;
};