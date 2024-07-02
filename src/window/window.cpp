// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "window.h"

bool Window::isCloseRequested()
{
    return state.bIsCloseRequested;
}

bool Window::isFullscreen()
{
    return state.bIsFullscreen;
}

int Window::getWidth()
{
    return state.realWidth;
}

int Window::getHeight()
{
    return state.realHeight;
}

void Window::close()
{
    state.bIsCloseRequested = true;
}

void Window::udpateRealSize(int width, int height)
{
    state.realWidth = width;
    state.realHeight = height;
}