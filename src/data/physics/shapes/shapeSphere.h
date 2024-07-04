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
    EXPORT Matrix3 &getInertiaTensor() override final { return inertia; }
    EXPORT AABB getAABB(Matrix4 *model) override final;

protected:
    float radius;
    Vector3 center;
    Matrix3 inertia;
};
