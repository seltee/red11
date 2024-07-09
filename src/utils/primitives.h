// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/glm/mat3x3.hpp"
#include "utils/glm/mat4x4.hpp"
#include "utils/glm/vec4.hpp"
#include "utils/glm/gtc/quaternion.hpp"
#include "utils/glm/gtx/quaternion.hpp"
#include "utils/glm/gtc/type_ptr.hpp"
#include "utils/color.h"

#include <math.h>

typedef glm::vec4 Vector4;
typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;

typedef glm::mat3x3 Matrix3;
typedef glm::mat4x4 Matrix4;

typedef glm::quat Quat;

class Actor;

struct PointWithDirection
{
    Vector3 vPosition;
    Vector3 vDirection;
};

struct Triangle
{
    Vector3 a;
    Vector3 b;
    Vector3 c;
    Vector3 n;
};

struct HullPolygon
{
    int index;
    unsigned short int points[7];
    unsigned short int pointsAmount;
    Vector3 normal;
};

struct HullEdge
{
    int a;
    int b;
    int polygon;
};

struct PolygonTriPoints
{
    unsigned int a, b, c;
};

struct RayCollisionPoint
{
    Vector3 point;
    Vector3 normal;
    float distance;
};

struct CollisionPoint
{
    Vector3 point;
    Vector3 normal;
    float depth;
    bool hasCollision;
};

struct PhysicsBodyPoint
{
    void *userData;
    Vector3 point;
    Vector3 normal;
    float distance;
};

struct FaceQuery
{
    float separation;
    HullPolygon *polygon;
    Vector3 axis;
};

struct EdgeQuery
{
    float separation;
    HullEdge *edgeA;
    HullEdge *edgeB;
    Vector3 axis;
};
