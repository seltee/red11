// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapePlain.h"

ShapePlain::ShapePlain(const Vector3 &normal, float distance)
{
    this->normal = glm::normalize(normal);
    this->distance = distance;
    this->mass = 9999.0f;

    inertia[0] = Vector3(1.0f, 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, 1.0f, 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, 1.0f);
}

ShapePlain::ShapePlain(const Vector3 &normal, const Vector3 &point)
{
    this->normal = glm::normalize(normal);
    this->distance = glm::dot(normal, point);
    this->mass = 9999.0f;
}

ShapeCollisionType ShapePlain::getType()
{
    return ShapeCollisionType::Plain;
}

AABB ShapePlain::getAABB(const Matrix4 &model)
{
    return AABB(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX), Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
}

int ShapePlain::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    float &distance = cache->plain.distance;
    Vector3 &normal = cache->plain.normal;
    // Compute the t value for the directed line ab intersecting the plane
    Vector3 ab = ray.b - ray.a;
    float t = (distance - glm::dot(normal, ray.a)) / glm::dot(normal, ab);
    // If t in [0..1] compute and return intersection point
    if (t >= 0.0f && t <= 1.0f)
    {
        newPoints[0] = PhysicsBodyPoint({nullptr, ray.a + t * ab, normal, t * glm::length(ab)});
        return 1;
    }
    return 0;
}
