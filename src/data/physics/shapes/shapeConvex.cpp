#include "shapeConvex.h"
#include <string>

ShapeConvex::ShapeConvex(float simScale, Vector3 *verticies, int verticiesAmount, HullPolygon *polygons, int polygonsAmount, float density)
{
    this->verticies = new Vector3[verticiesAmount];
    this->verticiesAmount = verticiesAmount;
    this->center = Vector3(0.0f);
    for (int i = 0; i < verticiesAmount; i++)
    {
        this->verticies[i] = verticies[i] * simScale;
    }

    mass = getConvexHullVolume(this->verticies, verticiesAmount) * density;
    inertia = getConvexHullInertia(this->verticies, verticiesAmount, density);

    this->polygons = new HullPolygon[polygonsAmount];
    this->polygonsAmount = polygonsAmount;
    memcpy(this->polygons, polygons, sizeof(HullPolygon) * polygonsAmount);
    for (int i = 0; i < verticiesAmount; i++)
    {
        float dist = glm::length2(this->verticies[i]);
        if (dist > aabbRadius)
            aabbRadius = dist;
    }
    aabbRadius = sqrtf(aabbRadius) * 1.4f;

    hullEdgesAmount = rebuildEdges(this->polygons, polygonsAmount, &hullEdges);

    /*
        BOX
        0.250000
        0.002604 0.000000 0.000000
        0.000000 0.002604 0.000000
        0.000000 0.000000 0.002604

        CONVEX
        0.114583
        0.002014 nan nan
        nan 0.002238 nan
        nan nan 0.001790
    */
    /*
        mass = 0.25f;
        inertia[0] = Vector3(0.002604, 0.000000, 0.000000);
        inertia[1] = Vector3(0.000000, 0.002604, 0.000000);
        inertia[2] = Vector3(0.000000, 0.000000, 0.002604);
    */
    /*
        printf("CONVEX\n");
        printf("%f\n", mass);
        printf("%f %f %f\n", inertia[0][0], inertia[0][1], inertia[0][2]);
        printf("%f %f %f\n", inertia[1][0], inertia[1][1], inertia[1][2]);
        printf("%f %f %f\n", inertia[2][0], inertia[2][1], inertia[2][2]);
    */
}

ShapeCollisionType ShapeConvex::getType()
{
    return ShapeCollisionType::Convex;
}

AABB ShapeConvex::getAABB(Matrix4 *model)
{
    Vector3 modelCenter = Vector3(*model * Vector4(center, 1.0f));
    return AABB(modelCenter - Vector3(aabbRadius, aabbRadius, aabbRadius), modelCenter + Vector3(aabbRadius, aabbRadius, aabbRadius));
}

int ShapeConvex::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
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
}