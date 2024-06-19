// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentCamera.h"

void ComponentCamera::onCreated()
{
}

void ComponentCamera::setupAsOrthographic(int width, int height, float nearDistance, float farDistance)
{
    camera.setupAsOrthographic(width, height, nearDistance, farDistance);
}

void ComponentCamera::setupAsPerspective(int width, int height, float nearDistance, float farDistance, float fov)
{
    camera.setupAsPerspective(width, height, nearDistance, farDistance, fov);
}

void ComponentCamera::onProcess(float delta)
{
    camera.updateViewMatrix(getModelMatrix());
}