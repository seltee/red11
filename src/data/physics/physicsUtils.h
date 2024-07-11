// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "collisionManifold.h"
#include <vector>
#include <list>

class PhysicsBody;
class CollisionDispatcher;
class CollisionCollector;

struct CollisionPair
{
    PhysicsBody *a;
    PhysicsBody *b;
    CollisionManifold manifold;
};

struct BodyPair
{
    PhysicsBody *a;
    PhysicsBody *b;
};

struct PhysicsBodyCacheTypeSphere
{
    float radius;
    Vector3 center;
};

struct PhysicsBodyCacheTypePlain
{
    float distance;
    Vector3 normal;
};

struct PhysicsBodyCacheTypeOBB
{
    Vector3 center;
    Vector3 axisX, axisY, axisZ;
    Vector3 points[8];
    Vector3 normals[6];
    Matrix4 transformation;
};

struct PhysicsBodyCacheTypeCapsule
{
    Vector3 a;
    Vector3 b;
    float radius;
};

struct PhysicsBodyCacheTypeConvex
{
    Vector3 center;
    Vector3 *verticies;
    Vector3 *normals;
};

union PhysicsBodyCache
{
    PhysicsBodyCacheTypeSphere sphere;
    PhysicsBodyCacheTypePlain plain;
    PhysicsBodyCacheTypeOBB OBB;
    PhysicsBodyCacheTypeCapsule capsule;
    PhysicsBodyCacheTypeConvex convex;
};

inline bool _compareBodyPoints(PhysicsBodyPoint a, PhysicsBodyPoint b)
{
    return (a.distance < b.distance);
}

void _prepareBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd);

void _finishBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd);

void _processBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd, float subStep, Vector3 localGravity);

void _applyStepBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd, float subStep);

void _collectPairs(
    std::vector<PhysicsBody *>::iterator bodyStart,
    std::vector<PhysicsBody *>::iterator bodyEnd,
    std::vector<PhysicsBody *> *bodyList,
    std::vector<BodyPair> *list);

void _collide(
    std::vector<BodyPair>::iterator pairStart,
    std::vector<BodyPair>::iterator pairEnd,
    CollisionDispatcher *collisionDispatcher,
    CollisionCollector *collisionCollector);

void _solve(
    std::vector<CollisionPair>::iterator pairStart,
    std::vector<CollisionPair>::iterator pairEnd,
    float simScale,
    float subStep);

void _ray(
    std::vector<PhysicsBody *>::iterator bodyStart,
    std::vector<PhysicsBody *>::iterator bodyEnd,
    const Segment &rayLocal,
    std::vector<PhysicsBodyPoint> *points);