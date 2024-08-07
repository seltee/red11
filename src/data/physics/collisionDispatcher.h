// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shapes/shape.h"
#include "collisionCollector.h"
#include "physicsBody.h"

typedef void (*CollectCollisions)(PhysicsBody *a, PhysicsBody *b, CollisionCollector *collector);

class CollisionDispatcher
{
public:
    CollisionDispatcher();
    EXPORT inline void collide(PhysicsBody *a, PhysicsBody *b, CollisionCollector *collector) { collectCollisions[(int)a->getType()][(int)b->getType()](a, b, collector); }

    static void collideSphereVsPlain(PhysicsBody *sphere, PhysicsBody *plain, CollisionCollector *collector);
    static void collideSphereVsSphere(PhysicsBody *sphereA, PhysicsBody *sphereB, CollisionCollector *collector);
    static void collideSphereVsMesh(PhysicsBody *sphere, PhysicsBody *mesh, CollisionCollector *collector);
    static void collideSphereVsConvex(PhysicsBody *sphere, PhysicsBody *convex, CollisionCollector *collector);

    static void collideOBBVsPlain(PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector);
    static void collideOBBVsSphere(PhysicsBody *OBB, PhysicsBody *sphere, CollisionCollector *collector);
    static void collideOBBVsMesh(PhysicsBody *OBB, PhysicsBody *mesh, CollisionCollector *collector);
    static void collideOBBVsOBB(PhysicsBody *OBB_A, PhysicsBody *OBB_B, CollisionCollector *collector);

    static void collideConvexVsConvex(PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector);
    static void collideConvexVsOBB(PhysicsBody *convex, PhysicsBody *OBB, CollisionCollector *collector);
    static void collideConvexVsPlain(PhysicsBody *convex, PhysicsBody *plain, CollisionCollector *collector);
    static void collideConvexVsMesh(PhysicsBody *convex, PhysicsBody *geometry, CollisionCollector *collector);

    static void collideCapsuleVsPlain(PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector);
    static void collideCapsuleVsCapsule(PhysicsBody *capsuleA, PhysicsBody *capsuleB, CollisionCollector *collector);
    static void collideCapsuleVsSphere(PhysicsBody *capsule, PhysicsBody *sphere, CollisionCollector *collector);
    static void collideCapsuleVsOBB(PhysicsBody *capsule, PhysicsBody *OBB, CollisionCollector *collector);
    static void collideCapsuleVsConvex(PhysicsBody *capsule, PhysicsBody *convex, CollisionCollector *collector);
    static void collideCapsuleVsMesh(PhysicsBody *capsule, PhysicsBody *mesh, CollisionCollector *collector);

    static void collideMeshVsMesh(PhysicsBody *meshA, PhysicsBody *meshB, CollisionCollector *collector);

protected:
    CollectCollisions collectCollisions[(int)ShapeCollisionType::Amount][(int)ShapeCollisionType::Amount];

    static HullEdge meshPolyEdges[6];
    static HullPolygon meshPolygon;
};