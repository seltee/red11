// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "utils/utils.h"
#include "utils/math.h"

class PhysicsWorld;

class ShapeSphere : public Shape
{
public:
    EXPORT ShapeSphere(const Vector3 &center, float radius, float density = 22.0f);
    inline float getRadius() { return radius; }

    EXPORT ShapeCollisionType getType() override final;
    EXPORT AABB getAABB(const Matrix4 &model) override final;

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) override final;

protected:
    float radius;
    Vector3 center;
};
