#include "shapeConvex.h"
#include <string>

ShapeConvex::ShapeConvex(const Vector3 &center, float simScale, Vector3 *verticies, int amount, float density)
{
    this->verticies = new Vector3[amount];
    for (int i = 0; i < amount; i++)
        this->verticies[i] = verticies[i] * simScale;

    mass = getConvexHullVolume(this->verticies, amount) * density;
    inertia = getConvexHullInertia(this->verticies, amount, density);
}

ShapeCollisionType ShapeConvex::getType()
{
    return ShapeCollisionType::Convex;
}

AABB ShapeConvex::getAABB(Matrix4 *model)
{
    return AABB(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX), Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
}

int ShapeConvex::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    return 0;
}