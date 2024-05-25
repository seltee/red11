// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"

enum class CameraType
{
    None,
    Orthographic,
    Perspective
};

class Camera
{
public:
    inline Matrix4 *getProjectionMatrix() { return &projectionMatrix; }
    inline Matrix4 *getViewMatrix() { return &viewMatrix; }
    inline int getWidth() { return width; }
    inline int getHeight() { return height; }

    EXPORT void setupAsOrthographic(int width, int height, float nearDistance = -1400.0f, float farDistance = 1400.0f);
    EXPORT void setupAsPerspective(int width, int height, float nearDistance = 0.1f, float farDistance = 100.0f, float fov = 45.0f);

    EXPORT void recalcCullingPlanes();
    inline Vector4 *getCullingPlanes() { return cullingPlanes; }

    inline void updateViewMatrix(Matrix4 *worldModelMatrix) { viewMatrix = glm::inverse(*worldModelMatrix); }
    EXPORT PointWithDirection screenToWorld(float x, float y, Matrix4 *worldModelMatrix);

    inline CameraType getType() { return type; }

protected:
    Vector4 cullingPlanes[6];
    Matrix4 projectionMatrix;
    Matrix4 viewMatrix;
    CameraType type = CameraType::None;

    int width = 0, height = 0;
    float farDistance = 100.0f, nearDistance = 0.1f;
    float perspFov = 45.0f;

    float orthoRight = -1.0f, orthoLeft = 1.0f, orthoBottom = -1.0f, orthoTop = 1.0f;
};