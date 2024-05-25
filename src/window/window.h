// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

struct WindowState
{
    int width;
    int height;

    bool bIsFulltscreen;
    bool bIsValid;
    bool bIsCloseRequested;
};

class Window
{
public:
    virtual void processWindow() = 0;

    EXPORT bool isCloseRequested();
    EXPORT bool isFullscreen();

    EXPORT int getWidth();
    EXPORT int getHeight();

protected:
    WindowState state;
};