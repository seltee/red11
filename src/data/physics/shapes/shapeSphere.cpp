// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeSphere.h"

ShapeSphere::ShapeSphere(const Vector3 &center, float radius, float density)
{
    this->radius = radius;
    this->center = center;

    mass = (4.0f / 3.0f) * CONST_PI * radius * radius * radius * density;

    float tenValue = 2.0f / 5.0f * mass * radius * radius;
    inertia[0] = Vector3(tenValue, 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, tenValue, 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, tenValue);
}

ShapeCollisionType ShapeSphere::getType()
{
    return ShapeCollisionType::Sphere;
}

AABB ShapeSphere::getAABB(const Matrix4 &model)
{
    Vector3 absoluteCenter = Vector3(model * Vector4(center, 1.0f));
    return AABB(absoluteCenter - radius, absoluteCenter + radius);
}

int ShapeSphere::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    float radius = cache->sphere.radius;
    Vector3 &center = cache->sphere.center;

    Vector3 bMinusA = ray.b - ray.a;
    Vector3 normal = glm::normalize(bMinusA);
    Vector3 m = ray.a - center;
    float b = glm::dot(m, normal);
    float c = glm::dot(m, m) - radius * radius;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return false;
    float discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return false;

    // Ray now found to intersect sphere, compute both points of intersection
    float sqrtDiscr = sqrtf(discr);
    float t1 = -b - sqrtDiscr;
    float t2 = -b + sqrtDiscr;
    float length = glm::length(bMinusA);

    // If t is negative, ray started inside sphere
    if (t1 < 0.0f)
    {
        if (t2 <= length)
        {
            Vector3 point = ray.a + t2 * normal;
            newPoints[0] = PhysicsBodyPoint({nullptr, point, glm::normalize(point - center), t2});
            return 1;
        }
    }
    else
    {
        int out = 0;
        if (t1 <= length)
        {
            Vector3 point = ray.a + t1 * normal;
            newPoints[0] = PhysicsBodyPoint({nullptr, point, glm::normalize(point - center), t1});
            out++;
        }
        if (t2 <= length)
        {
            Vector3 point = ray.a + t2 * normal;
            newPoints[out] = PhysicsBodyPoint({nullptr, point, glm::normalize(point - center), t2});
            out++;
        }
        return out;
    }

    return 0;
}

/*
void ShapeSphere::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness)
{

    for (int i = 0; i < 16; i++)
    {
        float lA = (float)i / 16.0f * CONST_PI * 2.0f;
        float lB = (float)(i + 1) / 16.0f * CONST_PI * 2.0f;

        debug->renderLine(
            (absoluteCenter + Vector3(radius * sinf(lA), radius * cosf(lA), 0.0f)) * scale,
            (absoluteCenter + Vector3(radius * sinf(lB), radius * cosf(lB), 0.0f)) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCenter + Vector3(0.0f, radius * sinf(lA), radius * cosf(lA))) * scale,
            (absoluteCenter + Vector3(0.0f, radius * sinf(lB), radius * cosf(lB))) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCenter + Vector3(radius * cosf(lA), 0.0f, radius * sinf(lA))) * scale,
            (absoluteCenter + Vector3(radius * cosf(lB), 0.0f, radius * sinf(lB))) * scale,
            projectionView, thickness, debugColorWireframe);
    }
}
*/