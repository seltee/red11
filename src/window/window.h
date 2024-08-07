// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"

#define WINDOW_FULLSCREEN 1
#define WINDOW_RESIZABLE 2
#define WINDOW_BORDERLESS 4

struct WindowState
{
    int requesedWidth;
    int requesedHeight;
    int realWidth;
    int realHeight;

    bool bIsFullscreen;
    bool bIsValid;
    bool bIsCloseRequested;
    bool bIsBorderless;
    bool bIsResizable;
};

enum class MouseCursorIcon
{
    None,
    Default,
    Hand,
    Wait,
    Cross,
    IBeam,
    Help
};

class Window
{
public:
    virtual void processWindow() = 0;

    EXPORT virtual void setFullscreen(bool fullscreenState) = 0;
    EXPORT virtual void setResolution(int width, int height) = 0;

    EXPORT bool isCloseRequested();
    EXPORT bool isFullscreen();

    EXPORT int getWidth();
    EXPORT int getHeight();

    EXPORT void close();

    // Called by OS related window class when resize occur
    EXPORT void udpateRealSize(int width, int height);

    virtual void setMousePosition(int x, int y, bool generateMoveEvents = false) = 0;
    virtual void setMousePosition(MousePosition position, bool generateMoveEvents = false) = 0;
    virtual MousePosition getMousePosition() = 0;
    virtual void setCursorVisibility(bool bState) = 0;
    virtual bool isFocused() = 0;
    virtual bool isCursorOverWindow() = 0;
    virtual void setCursorIcon(MouseCursorIcon icon, bool bForce = false) = 0;

protected:
    WindowState state;
};