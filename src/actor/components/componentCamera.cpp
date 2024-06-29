// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentCamera.h"

void ComponentCamera::onCreated()
{
}

void ComponentCamera::setupAsOrthographic(float width, float height, float nearDistance, float farDistance)
{
    camera.setupAsOrthographic(width, height, nearDistance, farDistance);
}

void ComponentCamera::setupAsPerspective(float width, float height, float nearDistance, float farDistance, float fov)
{
    camera.setupAsPerspective(width, height, nearDistance, farDistance, fov);
}

void ComponentCamera::onProcess(float delta)
{
    camera.updateViewMatrix(getModelMatrix());
}