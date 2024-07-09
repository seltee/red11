// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeOBB.h"

HullPolygon ShapeOBB::hullPolygons[6];
HullEdge *ShapeOBB::hullEdges = nullptr;
int ShapeOBB::hullEdgesAmount = 0;

ShapeOBB::ShapeOBB(const Vector3 &center, float width, float height, float depth, float density)
{
    this->center = center;

    this->width = width;
    this->height = height;
    this->depth = depth;

    this->halfWidth = width * 0.5f;
    this->halfHeight = height * 0.5f;
    this->halfDepth = depth * 0.5f;

    mass = width * height * depth * density;

    float width2 = width * width;
    float height2 = height * height;
    float depth2 = depth * depth;

    inertia[0] = Vector3((1.0f / 12.0f) * mass * (height2 + depth2), 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, (1.0f / 12.0f) * mass * (width2 + depth2), 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, (1.0f / 12.0f) * mass * (width2 + height2));

    if (hullEdgesAmount == 0)
        rebuildPolygons();
}

ShapeCollisionType ShapeOBB::getType()
{
    return ShapeCollisionType::OBB;
}

AABB ShapeOBB::getAABB(Matrix4 *model)
{
    Vector3 modelCenter = *model * Vector4(center, 1.0f);
    return AABB(modelCenter - Vector3(halfWidth, halfHeight, halfDepth) * 1.5f, modelCenter + Vector3(halfWidth, halfHeight, halfDepth) * 1.5f);
}

int ShapeOBB::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    Vector3 &center = cache->OBB.center;
    Matrix4 inverseTransformMatrix = glm::inverse(cache->OBB.transformation);

    Vector3 localRayStart = Vector3(inverseTransformMatrix * glm::vec4(ray.a, 1.0f));
    Vector3 localRayEnd = Vector3(inverseTransformMatrix * glm::vec4(ray.b, 1.0f));

    Vector3 bMinusA = localRayEnd - localRayStart;
    Vector3 invDir = 1.0f / bMinusA;

    Vector3 min = Vector3(-halfWidth, -halfHeight, -halfDepth);
    Vector3 max = Vector3(halfWidth, halfHeight, halfDepth);

    Vector3 t1 = (min - localRayStart) * invDir;
    Vector3 t2 = (max - localRayStart) * invDir;

    Vector3 tMin = glm::min(t1, t2);
    Vector3 tMax = glm::max(t1, t2);

    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

    if (tNear > tFar || tFar < 0.0f)
        return 0;

    if (tNear < 0.0f)
    {
        // Ray starts inside the box
        Vector3 point = Vector3(cache->OBB.transformation * Vector4(localRayStart + tFar * bMinusA, 1.0f));
        newPoints[0] = PhysicsBodyPoint({nullptr, point, glm::normalize(center - point), tFar});
        return 1;
    }
    else
    {
        float length = glm::length(bMinusA);
        int out = 0;
        if (tNear <= length)
        {
            Vector3 point = Vector3(cache->OBB.transformation * Vector4(localRayStart + tNear * bMinusA, 1.0f));
            newPoints[0] = PhysicsBodyPoint({nullptr, point, glm::normalize(center - point), tNear});
            out++;
        }
        if (tFar <= length)
        {
            Vector3 point = Vector3(cache->OBB.transformation * Vector4(localRayStart + tFar * bMinusA, 1.0f));
            newPoints[out] = PhysicsBodyPoint({nullptr, point, glm::normalize(center - point), tFar});
            out++;
        }
        return out;
    }

    return 0;
}

Vector3 ShapeOBB::getClosestPoint(Matrix4 &OBBTransformation, Vector3 point)
{
    Matrix4 invTransformation = glm::inverse(OBBTransformation);
    Vector3 localPoint = Vector3(invTransformation * Vector4(point.x, point.y, point.z, 1.0f));
    Vector3 result;

    result.x = localPoint.x;
    if (result.x < -halfWidth)
        result.x = -halfWidth;
    if (result.x > halfWidth)
        result.x = halfWidth;

    result.y = localPoint.y;
    if (result.y < -halfHeight)
        result.y = -halfHeight;
    if (result.y > halfHeight)
        result.y = halfHeight;

    result.z = localPoint.z;
    if (result.z < -halfDepth)
        result.z = -halfDepth;
    if (result.z > halfDepth)
        result.z = halfDepth;

    return Vector3(OBBTransformation * Vector4(result.x, result.y, result.z, 1.0f));
}

void ShapeOBB::rebuildPolygons()
{
    hullPolygons[0].points[0] = 0;
    hullPolygons[0].points[1] = 1;
    hullPolygons[0].points[2] = 3;
    hullPolygons[0].points[3] = 2;
    hullPolygons[0].pointsAmount = 4;
    hullPolygons[0].index = 0;
    hullPolygons[0].normal = Vector3(0.0f, 1.0f, 0.0f);

    hullPolygons[1].points[0] = 5;
    hullPolygons[1].points[1] = 4;
    hullPolygons[1].points[2] = 6;
    hullPolygons[1].points[3] = 7;
    hullPolygons[1].pointsAmount = 4;
    hullPolygons[1].index = 1;
    hullPolygons[1].normal = Vector3(0.0f, -1.0f, 0.0f);

    hullPolygons[2].points[0] = 1;
    hullPolygons[2].points[1] = 0;
    hullPolygons[2].points[2] = 4;
    hullPolygons[2].points[3] = 5;
    hullPolygons[2].pointsAmount = 4;
    hullPolygons[2].index = 2;
    hullPolygons[2].normal = Vector3(1.0f, 0.0f, 0.0f);

    hullPolygons[3].points[0] = 2;
    hullPolygons[3].points[1] = 3;
    hullPolygons[3].points[2] = 7;
    hullPolygons[3].points[3] = 6;
    hullPolygons[3].pointsAmount = 4;
    hullPolygons[3].index = 3;
    hullPolygons[3].normal = Vector3(-1.0f, 0.0f, 0.0f);

    hullPolygons[4].points[0] = 4;
    hullPolygons[4].points[1] = 0;
    hullPolygons[4].points[2] = 2;
    hullPolygons[4].points[3] = 6;
    hullPolygons[4].pointsAmount = 4;
    hullPolygons[4].index = 4;
    hullPolygons[4].normal = Vector3(0.0f, 0.0f, 1.0f);

    hullPolygons[5].points[0] = 7;
    hullPolygons[5].points[1] = 3;
    hullPolygons[5].points[2] = 1;
    hullPolygons[5].points[3] = 5;
    hullPolygons[5].pointsAmount = 4;
    hullPolygons[5].index = 5;
    hullPolygons[5].normal = Vector3(0.0f, 0.0f, -1.0f);

    hullEdgesAmount = rebuildEdges(hullPolygons, 6, &hullEdges);
}