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
    EXPORT WindowsWindow(const char *windowName, int width, int height, bool bIsFulltscreen);
    inline HWND getHwnd() { return hwnd; }

    EXPORT void processWindow();

    EXPORT void setMousePosition(int x, int y, bool generateMoveEvents = false);

protected:
    HWND hwnd;
    HINSTANCE hInstance;

    bool isWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor);
};

#endif