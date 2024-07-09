// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "utils/utils.h"
#include "utils/math.h"

class PhysicsWorld;

class ShapeConvex : public Shape
{
public:
    EXPORT ShapeConvex(const Vector3 &center, float simScale, Vector3 *verticies, int amount, float density);

    EXPORT ShapeCollisionType getType() override;
    EXPORT AABB getAABB(Matrix4 *model) override final;

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) override final;

    inline Vector3 *getVerticies() { return verticies; }
    inline int getVertexAmount() { return amount; }

protected:
    Vector3 center;
    Vector3 *verticies;
    int amount;
};
