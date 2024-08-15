// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeMesh.h"
#include <string>

ShapeMesh::ShapeMesh(float simScale, VertexDataUV *verticies, int verticiesAmount, PolygonTriPoints *polygons, int polygonsAmount, float density)
{
    // Verticies
    this->verticies = new Vector3[verticiesAmount];
    this->verticiesAmount = verticiesAmount;
    this->center = Vector3(0.0f);
    for (int i = 0; i < verticiesAmount; i++)
    {
        this->verticies[i] = verticies[i].position * simScale;
    }

    // Polygons
    this->polygons = new PolygonTriPoints[polygonsAmount];
    this->polygonsAmount = polygonsAmount;
    memcpy(this->polygons, polygons, sizeof(PolygonTriPoints) * polygonsAmount);

    // Normals
    this->normals = new Vector3[polygonsAmount];
    for (int i = 0; i < polygonsAmount; i++)
    {
        auto &poly = this->polygons[i];
        this->normals[i] = calculateNormal(this->verticies[poly.a], this->verticies[poly.b], this->verticies[poly.c]);
    }

    // aabb radius
    for (int i = 0; i < verticiesAmount; i++)
    {
        float dist = glm::length2(this->verticies[i]);
        if (dist > aabbRadius)
            aabbRadius = dist;
    }
    aabbRadius = sqrtf(aabbRadius);

    mass = (4.0f / 3.0f) * CONST_PI * aabbRadius * aabbRadius * aabbRadius * density * 0.5f;

    inertia[0] = Vector3(0.002604, 0.000000, 0.000000);
    inertia[1] = Vector3(0.000000, 0.002604, 0.000000);
    inertia[2] = Vector3(0.000000, 0.000000, 0.002604);

    // hullEdgesAmount = rebuildEdges(this->polygons, polygonsAmount, &hullEdges);
}

ShapeCollisionType ShapeMesh::getType()
{
    return ShapeCollisionType::Mesh;
}

AABB ShapeMesh::getAABB(Matrix4 *model)
{
    Vector3 modelCenter = Vector3(*model * Vector4(center, 1.0f));
    return AABB(modelCenter - Vector3(aabbRadius, aabbRadius, aabbRadius), modelCenter + Vector3(aabbRadius, aabbRadius, aabbRadius));
}

int ShapeMesh::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    Vector3 locRayA = Vector3(cache->mesh.invTransformation * Vector4(ray.a, 1.0f));
    Vector3 locRayB = Vector3(cache->mesh.invTransformation * Vector4(ray.b, 1.0f));
    Segment locRay(locRayA, locRayB);
    Matrix3 rotationScaleMatrix = Matrix3(cache->mesh.transformation);

    float distance;
    Vector3 point;
    Vector3 v[3];
    int pCount = 0;

    for (int i = 0; i < polygonsAmount; i++)
    {
        auto &poly = polygons[i];
        v[0] = verticies[poly.a];
        v[1] = verticies[poly.b];
        v[2] = verticies[poly.c];

        if (testRayAgainstTriangle(v, locRay, distance, point))
        {
            Vector3 normalOut = glm::normalize(rotationScaleMatrix * normals[i]);
            Vector3 pountOut = Vector3(cache->mesh.transformation * Vector4(point, 1.0f));

            newPoints[pCount] = PhysicsBodyPoint({nullptr, pountOut, normalOut, glm::length(ray.a - pountOut)});
            pCount++;

            // Only mesh can have unlimited collision points
            if (pCount == 8)
                break;
        }
    }

    return pCount;
}

Vector3 ShapeMesh::getClosestPoint(const Vector3 &point)
{
    float minDistance = FLT_MAX;
    Vector3 out(0.0f);
    for (int i = 0; i < this->polygonsAmount; i++)
    {
        PolygonTriPoints &p = this->polygons[i];
        Vector3 v[3] = {verticies[p.a], verticies[p.b], verticies[p.c]};

        Vector3 closest = getClosestPointOnTriangle(v, point);
        float distance = glm::length(closest - point);
        if (distance < minDistance)
        {
            minDistance = distance;
            out = closest;
        }
    }
    return out;
}

void ShapeMesh::getClosestPoint(Segment &segment, Vector3 *onSegment, Vector3 *onGeometry)
{
    float minDistance = FLT_MAX;
    Vector3 newOnSegment, newOnGeometry;

    for (int i = 0; i < this->polygonsAmount; i++)
    {
        PolygonTriPoints &p = this->polygons[i];
        Vector3 v[3] = {verticies[p.a], verticies[p.b], verticies[p.c]};

        float distance = segment.getClosestPointToTriangle(v, newOnGeometry, newOnSegment);
        if (distance < minDistance)
        {
            minDistance = distance;
            *onSegment = newOnSegment;
            *onGeometry = newOnGeometry;
        }
    }

    /*
    for (int i = 0; i < triCount; i++)
    {
        int s = i * 3;
        Vector3 newOnSegment, newOnGeometry;
        float distance = segment.getClosestPointToTriangle(&verticies[s], newOnGeometry, newOnSegment);
        if (distance < minDistance)
        {
            minDistance = distance;
            onSegment = newOnSegment;
            onGeometry = newOnGeometry;
        }
    }
    return minDistance;
    */
}