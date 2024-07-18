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
        this->normals[i] = getPolygonNormal(this->verticies[poly.a], this->verticies[poly.b], this->verticies[poly.c]);
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
    /*
    // Compute direction vector for the segment
    Vector3 direction = ray.b - ray.a;

    // Set initial interval to being the whole segment.
    float pointfirst = 0.0f;
    float pointlast = 1.0f;
    Vector3 normalOut = Vector3(0.0f, 1.0f, 0.0f);

    // Intersect segment against each polygon
    for (int p = 0; p < polygonsAmount; p++)
    {
        // Normal of the polygon = normal of it's plain
        Vector3 normal = cache->convex.normals[p];
        float denom = glm::dot(normal, direction);

        // Distance from center to polygon plane
        float plainDistance = glm::dot(normal, cache->convex.verticies[polygons[p].points[0]]);
        float lineOriginDistance = plainDistance - glm::dot(normal, ray.a);

        // Test if segment runs parallel to the plane
        if (denom == 0.0f)
        {
            if (lineOriginDistance > 0.0f)
                return 0;
        }
        else
        {
            // Compute parameterized t value for intersection with current plane
            float t = lineOriginDistance / denom;
            if (denom < 0.0f)
            {
                // Entering halfspace
                if (t > pointfirst)
                {
                    normalOut = normal;
                    pointfirst = t;
                }
            }
            else
            {
                // Exiting halfspace
                if (t < pointlast)
                {
                    normalOut = normal;
                    pointlast = t;
                }
            }
            // Exit with “no intersection” if intersection becomes empty
            if (pointfirst > pointlast)
                return 0;
        }
    }

    // The segment intersects the polyhedron, return first and last point
    // For polygedron there are always will be only 2 points
    float length = glm::length(direction);
    newPoints[0] = PhysicsBodyPoint({nullptr, ray.a + pointfirst * direction, normalOut, pointfirst * length});
    newPoints[1] = PhysicsBodyPoint({nullptr, ray.a + pointlast * direction, normalOut, pointlast * length});
    return 2;
    */
    return 0;
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