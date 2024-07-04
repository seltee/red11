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
    EXPORT ShapeSphere(const Vector3 &center, float radius);
    inline float getRadius() { return radius; }

    EXPORT ShapeCollisionType getType();
    EXPORT Matrix3 &getInertiaTensor() { return inertia; }
    inline Vector3 getAbsoluteCenter() { return absoluteCenter; }

protected:
    float radius;
    Vector3 center;
    Matrix3 inertia;
    Vector3 absoluteCenter;
};
