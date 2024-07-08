// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "utils/math.h"

class PhysicsWorld;

class ShapePlain : public Shape
{
public:
    EXPORT ShapePlain(const Vector3 &normal, float distance);
    EXPORT ShapePlain(const Vector3 &normal, const Vector3 &point);
    EXPORT ShapeCollisionType getType() override final;

    EXPORT Matrix3 &getInertiaTensor() override final { return inertia; }
    EXPORT AABB getAABB(Matrix4 *model) override final;

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) override final;

    inline Vector3 getClosestPoint(const Vector3 &point)
    {
        float t = glm::dot(normal, point) - distance;
        return point - t * normal;
    }

    inline Vector3 &getNormal() { return normal; }
    inline float getDistance() { return distance; }

protected:
    Vector3 normal;
    float distance;
    Matrix3 inertia;
};
