// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "primitives.h"

class Plain
{
public:
    Plain() {};
    Plain(const Vector3 &normal, float distance)
    {
        this->normal = normal;
        this->distance = distance;
    }

    inline Vector3 getClosestPoint(const Vector3 &point)
    {
        float t = glm::dot(normal, point) - distance;
        return point - t * normal;
    }

    Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);
    float distance = 0.0f;
};
