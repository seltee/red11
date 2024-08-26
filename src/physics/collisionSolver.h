// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "physicsBody.h"
#include "collisionManifold.h"

struct Collision
{
    PhysicsBody *a;
    PhysicsBody *b;
    CollisionPoint point;
};

class CollisionSolver
{
public:
    CollisionSolver(float simScale);

    void solve(PhysicsBody *a, PhysicsBody *b, CollisionManifold &manifold, float delta);

    float solveAxis(
        PhysicsBody *a,
        PhysicsBody *b,
        Vector3 pointA,
        Vector3 pointB,
        Vector3 velocityBodyA,
        Vector3 velocityBodyB,
        Vector3 angularVelocityBodyA,
        Vector3 angularVelocityBodyB,
        Vector3 axis,
        float fMin,
        float fMax,
        float bias);

protected:
    float simScale;
    float minRenormalVelocity;
    float totalLambda;
};
