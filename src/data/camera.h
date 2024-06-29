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
    inline Matrix4 *getWorldMatrix() { return &worldMatrix; }
    inline int getWidth() { return width; }
    inline int getHeight() { return height; }

    EXPORT void setupAsOrthographic(float width, float height, float nearDistance = -200.0f, float farDistance = 600.0f);
    EXPORT void setupAsPerspective(float width, float height, float nearDistance = 0.01f, float farDistance = 80.0f, float fov = 45.0f);

    EXPORT void recalcCullingPlanes();
    inline Vector4 *getCullingPlanes() { return cullingPlanes; }

    inline void updateViewMatrix(Matrix4 *worldModelMatrix)
    {
        this->worldMatrix = *worldModelMatrix;
        this->viewMatrix = glm::inverse(*worldModelMatrix);
    }
    inline Vector3 getForwardVector()
    {
        return Matrix3(worldMatrix) * Vector3(0.0f, 0.0f, -1.0f);
    }

    EXPORT PointWithDirection screenToWorld(float x, float y, Matrix4 *worldModelMatrix);

    inline CameraType getType() { return type; }

protected:
    Vector4 cullingPlanes[6];
    Matrix4 projectionMatrix;
    Matrix4 viewMatrix;
    Matrix4 worldMatrix;
    CameraType type = CameraType::None;

    float width = 0.0f, height = 0.0f;
    float farDistance = 100.0f, nearDistance = 0.1f;
    float perspFov = 45.0f;

    float orthoRight = -1.0f, orthoLeft = 1.0f, orthoBottom = -1.0f, orthoTop = 1.0f;
};