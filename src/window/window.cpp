// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "window.h"

bool Window::isCloseRequested()
{
    return state.bIsCloseRequested;
}

bool Window::isFullscreen()
{
    return state.bIsFulltscreen;
}

int Window::getWidth()
{
    return state.width;
}

int Window::getHeight()
{
    return state.height;
}