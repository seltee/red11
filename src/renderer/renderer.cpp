// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer.h"

Renderer::Renderer(Window *window)
{
    this->window = window;
    viewWidth = window->getWidth();
    viewHeight = window->getHeight();
}