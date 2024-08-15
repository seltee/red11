// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include <vector>

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

struct VideoMode
{
    unsigned int width;
    unsigned int height;
    unsigned int refreshRate;
};

class Window
{
public:
    virtual void processWindow() = 0;

    // Enables and disables fullscreen. Currently it works always as borderless in full resolution
    EXPORT virtual void setFullscreen(bool fullscreenState) = 0;

    // Sets resolution for windowed mode
    EXPORT virtual void setResolution(unsigned int width, unsigned int height) = 0;

    // Available for screen
    EXPORT virtual std::vector<VideoMode> getVideoModesList(unsigned int limitToFps = 0, bool sortByWidth = true) = 0;

    EXPORT virtual std::vector<unsigned int> getRefreshRatesList() = 0;
    EXPORT virtual std::vector<unsigned int> getRefreshRatesListForResolution(unsigned int width, unsigned int height) = 0;
    EXPORT virtual bool isResolutionAvailable(unsigned int width, unsigned int height, bool bIsInFullscreen) = 0;
    EXPORT virtual bool isResolutionAvailable(unsigned int width, unsigned int height, unsigned int refreshRate, bool bIsInFullscreen) = 0;

    EXPORT bool isCloseRequested();
    EXPORT bool isFullscreen();

    EXPORT int getWidth();
    EXPORT int getHeight();

    EXPORT void close();

    // Called by OS related window class when resize occur
    EXPORT void udpateRealSize(int width, int height);

    virtual void setMousePosition(int x, int y, bool generateMoveEvents = false) = 0;
    virtual void setMousePosition(const MousePosition &position, bool generateMoveEvents = false) = 0;
    virtual MousePosition getMousePosition() = 0;
    virtual void setCursorVisibility(bool bState) = 0;
    virtual bool isFocused() = 0;
    virtual bool isCursorOverWindow() = 0;
    virtual void setCursorIcon(MouseCursorIcon icon, bool bForce = false) = 0;

protected:
    WindowState state;
};