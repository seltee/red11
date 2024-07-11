// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "collisionDispatcher.h"
#include "shapes/shapePlain.h"
#include "shapes/shapeSphere.h"
/*
#include "physics/shapes/shapeBox.h"
#include "physics/shapes/shapeConvex.h"
#include "physics/shapes/shapeGeometry.h"
#include "physics/shapes/shapeCapsule.h"
#include "math/hullCliping.h"
*/
#include <vector>
#include "utils/algorithms.h"
#include "utils/hullCliping.h"

CollisionDispatcher::CollisionDispatcher()
{
    int amountOfTypes = (int)ShapeCollisionType::Amount;
    for (int a = 0; a < amountOfTypes; a++)
        for (int b = 0; b < amountOfTypes; b++)
            collectCollisions[a][b] = [](PhysicsBody *a, PhysicsBody *b, CollisionCollector *collector)
            {
                printf("Collision of %s with %s is not implemented\n", Shape::getTypeName(a->getType()).c_str(), Shape::getTypeName(b->getType()).c_str());
            };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *plain, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsPlain(sphere, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Plain] = [](PhysicsBody *sphere, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsPlain(sphere, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *sphere1, PhysicsBody *sphere2, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsSphere(sphere1, sphere2, collector);
    };

    /*

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *sphere, PhysicsBody *geometry, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsGeometry(sphere, geometry, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *geometry, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsGeometry(sphere, geometry, collector);
    };

    */
    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Convex] = [](PhysicsBody *sphere, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsConvex(sphere, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *convex, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideSphereVsConvex(sphere, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Plain] = [](PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsPlain(OBB, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::OBB] = [](PhysicsBody *plain, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsPlain(OBB, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *OBB, PhysicsBody *Sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsSphere(OBB, Sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::OBB] = [](PhysicsBody *Sphere, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideOBBVsSphere(OBB, Sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::OBB] = [](PhysicsBody *OBB_A, PhysicsBody *OBB_B, CollisionCollector *collector)
    {
        // cache of OBB compatible with cache of convex
        CollisionDispatcher::collideOBBVsOBB(OBB_A, OBB_B, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Convex] = [](PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsConvex(convexA, convexB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::OBB] = [](PhysicsBody *convex, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsOBB(convex, OBB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Convex] = [](PhysicsBody *OBB, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsOBB(convex, OBB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Plain] = [](PhysicsBody *convex, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsPlain(convex, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::Convex] = [](PhysicsBody *plain, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideConvexVsPlain(convex, plain, collector);
    };

    /*
        collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *convex, PhysicsBody *geometry, CollisionCollector *collector)
        {
            CollisionDispatcher::collideConvexVsGeometry(convex, geometry, collector);
        };

        collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::Convex] = [](PhysicsBody *geometry, PhysicsBody *convex, CollisionCollector *collector)
        {
            CollisionDispatcher::collideConvexVsGeometry(convex, geometry, collector);
        };

        collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *OBB, PhysicsBody *geometry, CollisionCollector *collector)
        {
            CollisionDispatcher::collideConvexVsGeometry(OBB, geometry, collector);
        };

        collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::OBB] = [](PhysicsBody *geometry, PhysicsBody *OBB, CollisionCollector *collector)
        {
            CollisionDispatcher::collideConvexVsGeometry(OBB, geometry, collector);
        };

        */

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Plain] = [](PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsPlain(capsule, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Plain][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *plain, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsPlain(capsule, plain, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *capsuleA, PhysicsBody *capsuleB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsCapsule(capsuleA, capsuleB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Sphere] = [](PhysicsBody *capsule, PhysicsBody *sphere, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsSphere(capsule, sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Sphere][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *sphere, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsSphere(capsule, sphere, collector);
    };

    collectCollisions[(int)ShapeCollisionType::OBB][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *OBB, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsOBB(capsule, OBB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::OBB] = [](PhysicsBody *capsule, PhysicsBody *OBB, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsOBB(capsule, OBB, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Convex][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *convex, PhysicsBody *capsule, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsConvex(capsule, convex, collector);
    };

    collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Convex] = [](PhysicsBody *capsule, PhysicsBody *convex, CollisionCollector *collector)
    {
        CollisionDispatcher::collideCapsuleVsConvex(capsule, convex, collector);
    };

    /*

collectCollisions[(int)ShapeCollisionType::Capsule][(int)ShapeCollisionType::Geometry] = [](PhysicsBody *capsule, PhysicsBody *geometry, CollisionCollector *collector)
{
    CollisionDispatcher::collideCapsuleVsGeometry(capsule, geometry, collector);
};

collectCollisions[(int)ShapeCollisionType::Geometry][(int)ShapeCollisionType::Capsule] = [](PhysicsBody *geometry, PhysicsBody *capsule, CollisionCollector *collector)
{
    CollisionDispatcher::collideCapsuleVsGeometry(capsule, geometry, collector);
};

*/
}

void CollisionDispatcher::collideSphereVsPlain(PhysicsBody *sphere, PhysicsBody *plain, CollisionCollector *collector)
{
    Vector3 sphereCenter = sphere->getCenterOfMass();
    PhysicsBodyCacheTypeSphere *sphereData = sphere->getCacheSphere(0);
    PhysicsBodyCacheTypePlain *plainData = plain->getCachePlain(0);
    Plain plainShape = Plain(plainData->normal, plainData->distance);
    float radius = sphereData->radius;

    Vector3 point = plainShape.getClosestPoint(sphereCenter);
    Vector3 difference = point - sphereCenter;
    float distance = glm::length(difference);

    if (distance < radius)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(Vector3(sphereCenter + normal * radius), point, radius - distance, normal);
        collector->addBodyPair(sphere, plain, manifold);
    }
}

void CollisionDispatcher::collideSphereVsSphere(PhysicsBody *sphereA, PhysicsBody *sphereB, CollisionCollector *collector)
{
    ShapeSphere *sphereShapeA = (ShapeSphere *)sphereA->getForm()->getSimpleShape();
    ShapeSphere *sphereShapeB = (ShapeSphere *)sphereB->getForm()->getSimpleShape();

    Vector3 sphereAPosition = sphereA->getCenterOfMass();
    Vector3 sphereBPosition = sphereB->getCenterOfMass();

    float shapeARadius = sphereShapeA->getRadius();
    float shapeBRadius = sphereShapeB->getRadius();

    Vector3 difference = sphereBPosition - sphereAPosition;
    float distance = glm::length(difference);
    float radiusSumm = shapeARadius + shapeBRadius;

    if (distance < radiusSumm)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(Vector3(sphereAPosition + normal * shapeARadius), Vector3(sphereBPosition - normal * shapeBRadius), radiusSumm - distance, normal);
        collector->addBodyPair(sphereA, sphereB, manifold);
    }
}

void CollisionDispatcher::collideOBBVsPlain(PhysicsBody *OBB, PhysicsBody *plain, CollisionCollector *collector)
{
    ShapeOBB *OBBShape = (ShapeOBB *)OBB->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeOBB *OBBData = OBB->getCacheOBB(0);
    PhysicsBodyCacheTypePlain *plainData = plain->getCachePlain(0);

    Vector3 &axisX = OBBData->axisX;
    Vector3 &axisY = OBBData->axisY;
    Vector3 &axisZ = OBBData->axisZ;

    Vector3 plainNormal = plainData->normal;

    float halfWidth = OBBShape->getHalfWidth();
    float halfHeight = OBBShape->getHalfHeight();
    float halfDepth = OBBShape->getHalfDepth();
    float r = halfWidth * abs(glm::dot(plainNormal, axisX)) +
              halfHeight * abs(glm::dot(plainNormal, axisY)) +
              halfDepth * abs(glm::dot(plainNormal, axisZ));

    Vector3 OBBCenter = OBBData->center;

    // Compute distance of box center from plane
    float s = glm::dot(plainNormal, OBBCenter) - plainData->distance;

    // Intersection occurs when distance s falls within [-r,+r] interval
    if (abs(s) <= r)
    {
        Plain plainShape = Plain(plainData->normal, plainData->distance);
        Vector3 closestToCenter = plainShape.getClosestPoint(OBBCenter);
        Vector3 sideNormal = glm::normalize(OBBCenter - closestToCenter);

        float n = 0.0f;
        Vector3 c = Vector3(0.0f);
        float depth = 0.0f;
        for (int i = 0; i < 8; i++)
        {
            Vector3 &p = OBBData->points[i];
            Vector3 closest = plainShape.getClosestPoint(p);
            Vector3 difference = p - closest;

            if (glm::dot(sideNormal, difference) < 0.0f)
            {
                c += p;
                n += 1.0f;
                depth = fmaxf(glm::length(difference), depth);
            }
        }
        if (n > 0.0f)
        {
            c /= n;

            CollisionManifold manifold;
            manifold.addCollisionPoint(c, plainShape.getClosestPoint(c), depth, -sideNormal);
            collector->addBodyPair(OBB, plain, manifold);
        }
    }
}

void CollisionDispatcher::collideOBBVsSphere(PhysicsBody *OBB, PhysicsBody *sphere, CollisionCollector *collector)
{
    ShapeOBB *OBBShape = (ShapeOBB *)OBB->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeOBB *OBBData = OBB->getCacheOBB(0);
    PhysicsBodyCacheTypeSphere *sphereData = sphere->getCacheSphere(0);

    Vector3 &sphereCenter = sphereData->center;
    float sphereRadius = sphereData->radius;

    Vector3 closestPoint = OBBShape->getClosestPoint(OBBData->transformation, sphereCenter);
    Vector3 difference = sphereCenter - closestPoint;

    float distance = glm::length(difference);
    if (distance < sphereRadius)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(sphereCenter + normal * sphereRadius, closestPoint, sphereRadius - distance, normal);
        collector->addBodyPair(OBB, sphere, manifold);
    }
}

void CollisionDispatcher::collideOBBVsOBB(PhysicsBody *OBB_A, PhysicsBody *OBB_B, CollisionCollector *collector)
{
    ShapeOBB *OBBShapeA = (ShapeOBB *)OBB_A->getForm()->getSimpleShape();
    ShapeOBB *OBBShapeB = (ShapeOBB *)OBB_B->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeOBB *OBBDataA = OBB_A->getCacheOBB(0);
    PhysicsBodyCacheTypeOBB *OBBDataB = OBB_B->getCacheOBB(0);

    FaceQuery faceQueryA = queryFaceDirection(OBBShapeA->getPolygons(), 6, OBBDataA->points, OBBDataA->normals, OBBDataB->points, 8);
    if (faceQueryA.separation > 0.0f)
        return;

    FaceQuery faceQueryB = queryFaceDirection(OBBShapeB->getPolygons(), 6, OBBDataB->points, OBBDataB->normals, OBBDataA->points, 8);
    if (faceQueryB.separation > 0.0f)
        return;

    EdgeQuery edgeQuery = queryEdgeDirection(
        OBBDataA->center,
        OBBShapeA->getEdges(),
        OBBShapeA->getEdgesAmount(),
        OBBDataA->points,
        OBBDataA->normals,
        OBBShapeB->getEdges(),
        OBBShapeB->getEdgesAmount(),
        OBBDataB->points,
        OBBDataB->normals);

    if (edgeQuery.separation > 0.0f)
        return;

    bool bIsFaceContactA = faceQueryA.separation > edgeQuery.separation;
    bool bIsFaceContactB = faceQueryB.separation > edgeQuery.separation;

    CollisionManifold manifold;
    if (bIsFaceContactA && bIsFaceContactB)
    {
        if (faceQueryA.separation > faceQueryB.separation)
        {
            HullCliping::clipHullAgainstHull(-faceQueryA.axis,
                                             OBBShapeA->getPolygons(),
                                             6,
                                             OBBDataA->points,
                                             8,
                                             OBBDataA->normals,
                                             OBBShapeB->getPolygons(),
                                             6,
                                             OBBDataB->points,
                                             8,
                                             OBBDataB->normals,
                                             &manifold);
        }
        else
        {
            HullCliping::clipHullAgainstHull(faceQueryB.axis,
                                             OBBShapeA->getPolygons(),
                                             6,
                                             OBBDataA->points,
                                             8,
                                             OBBDataA->normals,
                                             OBBShapeB->getPolygons(),
                                             6,
                                             OBBDataB->points,
                                             8,
                                             OBBDataB->normals,
                                             &manifold);
        }
    }
    else
    {
        HullCliping::clipHullAgainstHull(-edgeQuery.axis,
                                         OBBShapeA->getPolygons(),
                                         6,
                                         OBBDataA->points,
                                         8,
                                         OBBDataA->normals,
                                         OBBShapeB->getPolygons(),
                                         6,
                                         OBBDataB->points,
                                         8,
                                         OBBDataB->normals,
                                         &manifold);
    }

    if (manifold.collisionAmount > 0)
    {
        collector->addBodyPair(OBB_A, OBB_B, manifold);
    }
}

void CollisionDispatcher::collideSphereVsConvex(PhysicsBody *sphere, PhysicsBody *convex, CollisionCollector *collector)
{

    ShapeConvex *convexShape = (ShapeConvex *)convex->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeConvex *convexData = convex->getCacheConvex(0);
    PhysicsBodyCacheTypeSphere *sphereData = sphere->getCacheSphere(0);

    Vector3 center = sphereData->center;
    float radius = sphereData->radius;
    Vector3 closest = getClosestPointToHull(center, convexData->verticies, convexShape->getPolygons(), convexShape->getPolygonsAmount());

    Vector3 difference = closest - center;
    float distance = glm::length(difference);

    if (distance < radius)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        Vector3 collisionPoint = Vector3(center + normal * radius);
        manifold.addCollisionPoint(collisionPoint, closest, radius - distance, normal);
        collector->addBodyPair(sphere, convex, manifold);
    }
}

// SAT Hull vs Hull
// Dirk Gregorius - Robust Contact Creation for Physics Simulations, Valve Software
void CollisionDispatcher::collideConvexVsConvex(PhysicsBody *convexA, PhysicsBody *convexB, CollisionCollector *collector)
{
    ShapeConvex *shapeConvexA = (ShapeConvex *)convexA->getForm()->getSimpleShape();
    ShapeConvex *shapeConvexB = (ShapeConvex *)convexB->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeConvex *convexDataA = convexA->getCacheConvex(0);
    PhysicsBodyCacheTypeConvex *convexDataB = convexB->getCacheConvex(0);

    FaceQuery faceQueryA = queryFaceDirection(
        shapeConvexA->getPolygons(),
        shapeConvexA->getPolygonsAmount(),
        convexDataA->verticies,
        convexDataA->normals,
        convexDataB->verticies,
        shapeConvexB->getVerticiesAmount());

    if (faceQueryA.separation > 0.0f)
        return;

    FaceQuery faceQueryB = queryFaceDirection(
        shapeConvexB->getPolygons(),
        shapeConvexB->getPolygonsAmount(),
        convexDataB->verticies,
        convexDataB->normals,
        convexDataA->verticies,
        shapeConvexA->getVerticiesAmount());

    if (faceQueryB.separation > 0.0f)
        return;

    EdgeQuery edgeQuery = queryEdgeDirection(
        convexDataA->center,
        shapeConvexA->getEdges(),
        shapeConvexA->getEdgesAmount(),
        convexDataA->verticies,
        convexDataA->normals,
        shapeConvexB->getEdges(),
        shapeConvexB->getEdgesAmount(),
        convexDataB->verticies,
        convexDataB->normals);

    if (edgeQuery.separation > 0.0f)
        return;

    bool bIsFaceContactA = faceQueryA.separation > edgeQuery.separation;
    bool bIsFaceContactB = faceQueryB.separation > edgeQuery.separation;

    CollisionManifold manifold;
    if (bIsFaceContactA && bIsFaceContactB)
    {
        if (faceQueryA.separation > faceQueryB.separation)
        {
            HullCliping::clipHullAgainstHull(-faceQueryA.axis,
                                             shapeConvexA->getPolygons(),
                                             shapeConvexA->getPolygonsAmount(),
                                             convexDataA->verticies,
                                             shapeConvexA->getVerticiesAmount(),
                                             convexDataA->normals,
                                             shapeConvexB->getPolygons(),
                                             shapeConvexB->getPolygonsAmount(),
                                             convexDataB->verticies,
                                             shapeConvexB->getVerticiesAmount(),
                                             convexDataB->normals,
                                             &manifold);
        }
        else
        {
            HullCliping::clipHullAgainstHull(faceQueryB.axis,
                                             shapeConvexA->getPolygons(),
                                             shapeConvexA->getPolygonsAmount(),
                                             convexDataA->verticies,
                                             shapeConvexA->getVerticiesAmount(),
                                             convexDataA->normals,
                                             shapeConvexB->getPolygons(),
                                             shapeConvexB->getPolygonsAmount(),
                                             convexDataB->verticies,
                                             shapeConvexB->getVerticiesAmount(),
                                             convexDataB->normals,
                                             &manifold);
        }
    }
    else
    {
        HullCliping::clipHullAgainstHull(-edgeQuery.axis,
                                         shapeConvexA->getPolygons(),
                                         shapeConvexA->getPolygonsAmount(),
                                         convexDataA->verticies,
                                         shapeConvexA->getVerticiesAmount(),
                                         convexDataA->normals,
                                         shapeConvexB->getPolygons(),
                                         shapeConvexB->getPolygonsAmount(),
                                         convexDataB->verticies,
                                         shapeConvexB->getVerticiesAmount(),
                                         convexDataB->normals,
                                         &manifold);
    }

    if (manifold.collisionAmount > 0)
        collector->addBodyPair(convexA, convexB, manifold);
}

void CollisionDispatcher::collideConvexVsOBB(PhysicsBody *convex, PhysicsBody *OBB, CollisionCollector *collector)
{
    ShapeConvex *shapeConvex = (ShapeConvex *)convex->getForm()->getSimpleShape();
    ShapeOBB *OBBShape = (ShapeOBB *)OBB->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeConvex *convexData = convex->getCacheConvex(0);
    PhysicsBodyCacheTypeOBB *OBBData = OBB->getCacheOBB(0);

    FaceQuery faceQueryA = queryFaceDirection(
        shapeConvex->getPolygons(),
        shapeConvex->getPolygonsAmount(),
        convexData->verticies,
        convexData->normals,
        OBBData->points,
        8);

    if (faceQueryA.separation > 0.0f)
        return;

    FaceQuery faceQueryB = queryFaceDirection(
        OBBShape->getPolygons(),
        6,
        OBBData->points,
        OBBData->normals,
        convexData->verticies,
        shapeConvex->getVerticiesAmount());

    if (faceQueryB.separation > 0.0f)
        return;

    EdgeQuery edgeQuery = queryEdgeDirection(
        convexData->center,
        shapeConvex->getEdges(),
        shapeConvex->getEdgesAmount(),
        convexData->verticies,
        convexData->normals,
        OBBShape->getEdges(),
        OBBShape->getEdgesAmount(),
        OBBData->points,
        OBBData->normals);

    if (edgeQuery.separation > 0.0f)
        return;

    bool bIsFaceContactA = faceQueryA.separation > edgeQuery.separation;
    bool bIsFaceContactB = faceQueryB.separation > edgeQuery.separation;

    CollisionManifold manifold;
    if (bIsFaceContactA && bIsFaceContactB)
    {
        if (faceQueryA.separation > faceQueryB.separation)
        {
            HullCliping::clipHullAgainstHull(-faceQueryA.axis,
                                             shapeConvex->getPolygons(),
                                             shapeConvex->getPolygonsAmount(),
                                             convexData->verticies,
                                             shapeConvex->getVerticiesAmount(),
                                             convexData->normals,
                                             OBBShape->getPolygons(),
                                             6,
                                             OBBData->points,
                                             8,
                                             OBBData->normals,
                                             &manifold);
        }
        else
        {
            HullCliping::clipHullAgainstHull(faceQueryB.axis,
                                             shapeConvex->getPolygons(),
                                             shapeConvex->getPolygonsAmount(),
                                             convexData->verticies,
                                             shapeConvex->getVerticiesAmount(),
                                             convexData->normals,
                                             OBBShape->getPolygons(),
                                             6,
                                             OBBData->points,
                                             8,
                                             OBBData->normals,
                                             &manifold);
        }
    }
    else
    {
        HullCliping::clipHullAgainstHull(-edgeQuery.axis,
                                         shapeConvex->getPolygons(),
                                         shapeConvex->getPolygonsAmount(),
                                         convexData->verticies,
                                         shapeConvex->getVerticiesAmount(),
                                         convexData->normals,
                                         OBBShape->getPolygons(),
                                         6,
                                         OBBData->points,
                                         8,
                                         OBBData->normals,
                                         &manifold);
    }

    if (manifold.collisionAmount > 0)
        collector->addBodyPair(convex, OBB, manifold);
}

void CollisionDispatcher::collideConvexVsPlain(PhysicsBody *convex, PhysicsBody *plain, CollisionCollector *collector)
{
    ShapeConvex *convexShape = (ShapeConvex *)convex->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeConvex *convexData = convex->getCacheConvex(0);
    PhysicsBodyCacheTypePlain *plainData = plain->getCachePlain(0);

    Plain plainShape = Plain(plainData->normal, plainData->distance);
    Vector3 convexCenter = convexData->center;
    Vector3 closestToCenter = plainShape.getClosestPoint(convexCenter);
    Vector3 sideNormal = glm::normalize(convexCenter - closestToCenter);

    float n = 0.0f;
    Vector3 c = Vector3(0.0f);
    float depth = 0.0f;
    Vector3 *verticies = convexData->verticies;
    for (int i = 0; i < convexShape->getVerticiesAmount(); i++)
    {
        Vector3 &p = verticies[i];
        Vector3 closest = plainShape.getClosestPoint(p);
        Vector3 difference = p - closest;

        if (glm::dot(sideNormal, difference) < 0.0f)
        {
            c += p;
            n += 1.0f;
            depth = fmaxf(glm::length(difference), depth);
        }
    }
    if (n > 0.0f)
    {
        c /= n;
        CollisionManifold manifold;
        manifold.addCollisionPoint(c, plainShape.getClosestPoint(c), depth, -sideNormal);
        collector->addBodyPair(convex, plain, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsPlain(PhysicsBody *capsule, PhysicsBody *plain, CollisionCollector *collector)
{
    PhysicsBodyCacheTypeCapsule *capsuleData = capsule->getCacheCapsule(0);
    PhysicsBodyCacheTypePlain *plainData = plain->getCachePlain(0);

    float radius = capsuleData->radius;
    Vector3 &pointA = capsuleData->a;
    Vector3 &pointB = capsuleData->b;

    Plain plainShape = Plain(plainData->normal, plainData->distance);

    Vector3 closestA = plainShape.getClosestPoint(pointA);
    Vector3 closestB = plainShape.getClosestPoint(pointB);

    float distA = glm::length(pointA - closestA);
    float distB = glm::length(pointB - closestB);

    Vector3 point, segmentPoint;
    float distance;
    if (fabsf(distA - distB) < 0.0001f)
    {
        point = (closestA + closestB) * 0.5f;
        segmentPoint = (pointA + pointB) * 0.5f;
        distance = distA;
    }
    else
    {
        if (distA < distB)
        {
            point = closestA;
            segmentPoint = pointA;
            distance = distA;
        }
        else
        {
            point = closestB;
            segmentPoint = pointB;
            distance = distB;
        }
    }

    if (distance < radius)
    {
        CollisionManifold manifold;
        Vector3 normal = glm::normalize(segmentPoint - point);
        manifold.addCollisionPoint(point, segmentPoint - normal * radius, radius - distance, normal);
        collector->addBodyPair(plain, capsule, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsCapsule(PhysicsBody *capsuleA, PhysicsBody *capsuleB, CollisionCollector *collector)
{

    PhysicsBodyCacheTypeCapsule *capsuleDataA = capsuleA->getCacheCapsule(0);
    PhysicsBodyCacheTypeCapsule *capsuleDataB = capsuleB->getCacheCapsule(0);

    Segment segmentA = Segment(capsuleDataA->a, capsuleDataA->b);
    Segment segmentB = Segment(capsuleDataB->a, capsuleDataB->b);

    float s, t;
    Vector3 onA, onB;
    getClosestPointSegmentSegment(segmentA.a, segmentA.b, segmentB.a, segmentB.b, s, t, onA, onB);
    float distance = glm::length(onA - onB);

    float rSumm = capsuleDataA->radius + capsuleDataB->radius;
    if (distance < rSumm)
    {
        CollisionManifold manifold;
        Vector3 normal = glm::normalize(onB - onA);
        manifold.addCollisionPoint(onA + normal * capsuleDataA->radius, onB - normal * capsuleDataB->radius, rSumm - distance, normal);
        collector->addBodyPair(capsuleA, capsuleB, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsSphere(PhysicsBody *capsule, PhysicsBody *sphere, CollisionCollector *collector)
{
    PhysicsBodyCacheTypeCapsule *capsuleData = capsule->getCacheCapsule(0);
    PhysicsBodyCacheTypeSphere *sphereData = sphere->getCacheSphere(0);

    Segment segment = Segment(capsuleData->a, capsuleData->b);
    Vector3 sphereCenter = sphereData->center;

    Vector3 closest = segment.getClosestPoint(sphereCenter);
    Vector3 difference = sphereCenter - closest;
    float distance = glm::length(difference);
    float rSumm = sphereData->radius + capsuleData->radius;
    if (distance < rSumm)
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        manifold.addCollisionPoint(closest + normal * capsuleData->radius, sphereCenter - normal * sphereData->radius, rSumm - distance, normal);
        collector->addBodyPair(capsule, sphere, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsOBB(PhysicsBody *capsule, PhysicsBody *OBB, CollisionCollector *collector)
{
    ShapeOBB *OBBShape = (ShapeOBB *)OBB->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeCapsule *capsuleData = capsule->getCacheCapsule(0);
    PhysicsBodyCacheTypeOBB *OBBData = OBB->getCacheOBB(0);

    float radius = capsuleData->radius;

    Vector3 verticiesCapsule[2] = {capsuleData->a, capsuleData->b};
    FaceQuery faceQuery = queryFaceDirection(OBBShape->getPolygons(), 6, OBBData->points, OBBData->normals, verticiesCapsule, 2);
    if (faceQuery.separation > radius)
        return;

    CollisionManifold manifold;
    Segment segment = Segment(capsuleData->a, capsuleData->b);

    if (faceQuery.separation > 0.0f)
    {
        // Outside
        Vector3 poly[16];
        int amount = faceQuery.polygon->pointsAmount;
        for (int i = 0; i < amount; i++)
        {
            poly[i] = OBBData->points[faceQuery.polygon->points[i]];
        }

        Vector3 onPoly, onSegment;

        float distance = segment.getClosestPointToPoly(poly, amount, onPoly, onSegment);
        Vector3 difference = onSegment - onPoly;

        if (distance < radius && distance > 0.0f)
        {
            Vector3 normal = difference / distance;
            manifold.addCollisionPoint(onPoly, onSegment + normal * radius, radius - distance, normal);
            collector->addBodyPair(OBB, capsule, manifold);
        }
    }
    else
    {
        // Inside
        Vector3 normal = faceQuery.axis;
        Vector3 closest = segment.getClosestPoint(OBBData->center);
        Vector3 point = closest + normal * radius;
        manifold.addCollisionPoint(point, point, -faceQuery.separation, normal);
        collector->addBodyPair(OBB, capsule, manifold);
    }
}

void CollisionDispatcher::collideCapsuleVsConvex(PhysicsBody *capsule, PhysicsBody *convex, CollisionCollector *collector)
{
    ShapeConvex *convexShape = (ShapeConvex *)convex->getForm()->getSimpleShape();
    PhysicsBodyCacheTypeCapsule *capsuleData = capsule->getCacheCapsule(0);
    PhysicsBodyCacheTypeConvex *convexData = convex->getCacheConvex(0);

    float radius = capsuleData->radius;

    Vector3 verticiesCapsule[2] = {capsuleData->a, capsuleData->b};
    FaceQuery faceQuery = queryFaceDirection(convexShape->getPolygons(),
                                             convexShape->getPolygonsAmount(),
                                             convexData->verticies,
                                             convexData->normals,
                                             verticiesCapsule,
                                             2);

    if (faceQuery.separation > radius)
        return;

    CollisionManifold manifold;
    Segment segment = Segment(capsuleData->a, capsuleData->b);

    if (faceQuery.separation > 0.0f)
    {
        // Outside
        Vector3 poly[16];
        int amount = faceQuery.polygon->pointsAmount;
        for (int i = 0; i < amount; i++)
            poly[i] = convexData->verticies[faceQuery.polygon->points[i]];

        Vector3 onPoly, onSegment;

        float distance = segment.getClosestPointToPoly(poly, amount, onPoly, onSegment);
        Vector3 difference = onSegment - onPoly;

        if (distance < radius && distance > 0.0f)
        {
            Vector3 normal = difference / distance;
            manifold.addCollisionPoint(onPoly, onSegment + normal * radius, radius - distance, normal);
            collector->addBodyPair(convex, capsule, manifold);
        }
    }
    else
    {
        // Inside
        Vector3 normal = faceQuery.axis;
        Vector3 closest = segment.getClosestPoint(convexData->center);
        Vector3 point = closest + normal * radius;
        manifold.addCollisionPoint(point, point, -faceQuery.separation, normal);
        collector->addBodyPair(convex, capsule, manifold);
    }
}

/*
void CollisionDispatcher::collideSphereVsGeometry(PhysicsBody *sphere, PhysicsBody *geometry, CollisionCollector *collector)
{
    ShapeSphere *sphereShape = (ShapeSphere *)sphere->getShape();
    ShapeGeometry *geometryShape = (ShapeGeometry *)geometry->getShape();

    Vector3 center = sphere->getCenterOfMass();
    Vector3 closest = geometryShape->getClosestPoint(center);

    Vector3 difference = closest - center;
    float distance = glm::length(difference);
    if (distance < sphereShape->getRadius())
    {
        CollisionManifold manifold;
        Vector3 normal = difference / distance;
        Vector3 collisionPoint = Vector3(center + normal * sphereShape->getRadius());
        manifold.addCollisionPoint(collisionPoint, closest, sphereShape->getRadius() - distance, normal);

        collector->addBodyPair(sphere, geometry, manifold);
    }
}


void CollisionDispatcher::collideConvexVsGeometry(PhysicsBody *convex, PhysicsBody *geometry, CollisionCollector *collector)
{
    ShapeConvex *convexShape = (ShapeConvex *)convex->getShape();
    ShapeGeometry *geometryShape = (ShapeGeometry *)geometry->getShape();
    Hull *convexHull = convexShape->getHull();
    if (!convexHull)
        return;

    ShapeConvex geometryToConvex(Vector3(0.0f), nullptr);
    Vector3 verticies[3] = {Vector3(0.0f), Vector3(1.0f), Vector3(2.0f)};
    Hull *hull = geometryToConvex.setNewHull(verticies, 3);
    int pointsA[3] = {0, 1, 2};
    hull->addPolygon(pointsA, 3);

    int pointsB[3] = {0, 2, 1};
    hull->addPolygon(pointsB, 3);

    hull->hullCenter = geometry->getCenterOfMass();

    auto amountVerticies = geometryShape->getVerticiesAmount();
    auto amountTries = amountVerticies / 3;
    auto tri = geometryShape->getAbsoluteVerticies();

    CollisionManifold manifold;
    for (int i = 0; i < amountTries; i++)
    {
        int polyInside = i * 3;

        hull->absoluteVerticies[0] = tri[polyInside];
        hull->absoluteVerticies[1] = tri[polyInside + 1];
        hull->absoluteVerticies[2] = tri[polyInside + 2];
        hull->rebuildEdges();
        hull->rebuildNormals();

        Vector3 center = (hull->absoluteVerticies[0] + hull->absoluteVerticies[1] + hull->absoluteVerticies[2]) / 3.0f;
        hull->hullCenter = center;

        FaceQuery faceQueryA = geometryToConvex.queryFaceDirection(convexShape);
        if (faceQueryA.separation > 0.0f)
            continue;

        FaceQuery faceQueryB = convexShape->queryFaceDirection(&geometryToConvex);
        if (faceQueryB.separation > 0.0f)
            continue;

        EdgeQuery edgeQuery = geometryToConvex.queryEdgeDirection(convexShape);
        if (edgeQuery.separation > 0.0f)
            continue;

        bool bIsFaceContactA = faceQueryA.separation > edgeQuery.separation;
        bool bIsFaceContactB = faceQueryB.separation > edgeQuery.separation;

        if (bIsFaceContactA && bIsFaceContactB)
        {
            if (faceQueryA.separation > faceQueryB.separation)
                HullCliping::clipHullAgainstHull(convexHull, hull, faceQueryA.axis, &manifold);
            else
                HullCliping::clipHullAgainstHull(convexHull, hull, -faceQueryB.axis, &manifold);
        }
        else
        {
            HullCliping::clipHullAgainstHull(convexHull, hull, edgeQuery.axis, &manifold);
        }
    }
    if (manifold.collisionAmount > 0)
    {
        // printf("Collisions: %i\n", manifold.collisionAmount);
        manifold.combineIntoOne();
        // printf("%f %f %f, %f\n", manifold.normal.x, manifold.normal.y, manifold.normal.z, manifold.depth);
        // printf("%f %f %f - %f %f %f\n", manifold.pointsOnA[0].x, manifold.pointsOnA[0].y, manifold.pointsOnA[0].z, manifold.pointsOnB[0].x, manifold.pointsOnB[0].y, manifold.pointsOnB[0].z);
        if (manifold.depth[0] > 0.0f)
            collector->addBodyPair(convex, geometry, manifold);
    }
}


void CollisionDispatcher::collideCapsuleVsGeometry(PhysicsBody *capsule, PhysicsBody *geometry, CollisionCollector *collector)
{
    ShapeCapsule *capsuleShape = (ShapeCapsule *)capsule->getShape();
    ShapeGeometry *geometryShape = (ShapeGeometry *)geometry->getShape();
    Segment segment = capsuleShape->getAbsoluteCapsule();

    float radius = capsuleShape->getRadius();

    Vector3 onSegment, onGeometry;
    float distance = geometryShape->getClosestPoint(segment, onSegment, onGeometry);

    if (distance < radius)
    {
        CollisionManifold manifold;

        Vector3 normal = glm::normalize(onGeometry - onSegment);
        manifold.addCollisionPoint(onSegment + normal * capsuleShape->getRadius(), onGeometry, radius - distance, normal);
        collector->addBodyPair(capsule, geometry, manifold);
    }
}

*/