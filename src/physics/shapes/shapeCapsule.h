// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "utils/utils.h"
#include "utils/math.h"

class PhysicsWorld;

class ShapeCapsule : public Shape
{
public:
    EXPORT ShapeCapsule(const Vector3 &a, const Vector3 &b, float radius, float density);

    EXPORT ShapeCollisionType getType() override final;
    EXPORT AABB getAABB(const Matrix4 &model) override final;

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) override final;

    inline Vector3 &getA() { return a; }
    inline Vector3 &getB() { return b; }

    inline float getRadius() { return radius; }
    inline float getHeight() { return height; }

protected:
    Vector3 a, b;
    Vector3 center;
    float radius;
    float height;
};
