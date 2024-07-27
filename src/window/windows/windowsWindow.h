// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <shellscalingapi.h>
#include <windows.h>

#include "window/window.h"

class WindowsWindow : public Window
{
public:
    EXPORT WindowsWindow(const char *windowName, int width, int height, int flags = 0);
    inline HWND getHwnd() { return hWnd; }

    EXPORT void processWindow() override final;

    EXPORT void setFullscreen(bool fullscreenState) override final;

    EXPORT void setResolution(int width, int height) override final;

    EXPORT void setMousePosition(int x, int y, bool generateMoveEvents = false) override final;
    EXPORT void setMousePosition(MousePosition position, bool generateMoveEvents = false) override final;
    EXPORT void updateMousePosition(int x, int y);
    EXPORT void updateMouseOverWindow(bool bState);
    EXPORT MousePosition getMousePosition() override final;

    EXPORT void setCursorVisibility(bool state) override final;

    EXPORT bool isFocused() override final;

    EXPORT int getStyleForState(WindowState &bState);

    EXPORT void setIsFocused(bool state);

    EXPORT float getAdditionalWindowWidth();
    EXPORT float getAdditionalWindowHeight();

    EXPORT bool isCursorOverWindow() override final;

    EXPORT void setCursorIcon(MouseCursorIcon icon, bool bForce = false) override final;
    EXPORT void resetCursorIcon();

protected:
    HWND hWnd;
    HINSTANCE hInstance;
    int mousePositionX = 0;
    int mousePositionY = 0;
    bool bIsFocused = true;
    bool bCursorShown = true;
    bool bCursorOverWindow = true;

    HCURSOR hCursorDefault;
    HCURSOR hCursorHand;
    HCURSOR hCursorWait;
    HCURSOR hCursorCross;
    HCURSOR hCursorIBeam;
    HCURSOR hCursorHelp;
    MouseCursorIcon mouseIcon = MouseCursorIcon::Default;
};

#endif