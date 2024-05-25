// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "camera.h"
#include "utils/glm/gtc/type_ptr.hpp"
#include "utils/glm/gtc/matrix_access.inl"

void Camera::setupAsOrthographic(int width, int height, float nearDistance, float farDistance)
{
    type = CameraType::Orthographic;
    this->width = width;
    this->height = height;
    this->nearDistance = nearDistance;
    this->farDistance = farDistance;

    orthoRight = (float)width / 2.0f;
    orthoLeft = -orthoRight;
    orthoBottom = -(float)height / 2.0f;
    orthoTop = -orthoBottom;

    projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoBottom, nearDistance, farDistance);
    viewMatrix = Matrix4(1.0f);

    recalcCullingPlanes();
}

void Camera::setupAsPerspective(int width, int height, float nearDistance, float farDistance, float fov)
{
    type = CameraType::Perspective;
    this->width = width;
    this->height = height;
    this->nearDistance = nearDistance;
    this->farDistance = farDistance;

    float aspect = (float)width / (float)height;

    this->perspFov = fov;

    projectionMatrix = glm::perspective(fov, aspect, nearDistance, farDistance);
    viewMatrix = Matrix4(1.0f);

    recalcCullingPlanes();
}

void Camera::recalcCullingPlanes()
{
    if (type == CameraType::Orthographic)
    {
        // Near and far planes
        cullingPlanes[0] = Vector4(0, 0, -1, -nearDistance); // Near
        cullingPlanes[1] = Vector4(0, 0, 1, farDistance);    // Far

        // Left and right planes
        cullingPlanes[2] = Vector4(1, 0, 0, -orthoLeft);  // Left
        cullingPlanes[3] = Vector4(-1, 0, 0, orthoRight); // Right

        // Bottom and top planes
        cullingPlanes[4] = Vector4(0, 1, 0, orthoTop);      // Top
        cullingPlanes[5] = Vector4(0, -1, 0, -orthoBottom); // Bottom
    }
    else
    {
        cullingPlanes[0] = glm::row(projectionMatrix, 3) + glm::row(projectionMatrix, 2); // Near
        cullingPlanes[1] = glm::row(projectionMatrix, 3) - glm::row(projectionMatrix, 2); // Far
        cullingPlanes[2] = glm::row(projectionMatrix, 3) + glm::row(projectionMatrix, 0); // Left
        cullingPlanes[3] = glm::row(projectionMatrix, 3) - glm::row(projectionMatrix, 0); // Right
        cullingPlanes[4] = glm::row(projectionMatrix, 3) - glm::row(projectionMatrix, 1); // Top
        cullingPlanes[5] = glm::row(projectionMatrix, 3) + glm::row(projectionMatrix, 1); // Bottom

        // Normalize the planes
        for (int i = 0; i < 6; ++i)
        {
            cullingPlanes[i] /= length(Vector3(cullingPlanes[i]));
        }
    }
}

PointWithDirection Camera::screenToWorld(float x, float y, Matrix4 *worldModelMatrix)
{
    if (worldModelMatrix)
        updateViewMatrix(worldModelMatrix);

    PointWithDirection out;

    float halfWidth = (float)(width) / 2.0f;
    float halfHeight = (float)(height) / 2.0f;

    Matrix4 mView = glm::inverse(projectionMatrix * viewMatrix);

    Vector4 near = Vector4((x - halfWidth) / halfWidth, -1 * (y - halfHeight) / halfHeight, 0.0f, 1.0);
    Vector4 far = Vector4((x - halfWidth) / halfWidth, -1 * (y - halfHeight) / halfHeight, 1.0f, 1.0);

    Vector4 nearResult = mView * near;
    Vector4 farResult = mView * far;
    nearResult /= nearResult.w;
    farResult /= farResult.w;

    out.vDirection = glm::normalize(Vector3(farResult - nearResult));

    out.vPosition = Vector3({nearResult.x,
                             nearResult.y,
                             nearResult.z});

    return out;
}