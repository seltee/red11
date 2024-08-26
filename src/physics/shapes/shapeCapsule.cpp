// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeCapsule.h"

// Function to compute the rotation quaternion from the capsule's axis
Quat computeRotationQuaternion(const Vector3 &pointA, const Vector3 &pointB)
{
    Vector3 axis = glm::normalize(pointB - pointA);
    Vector3 up(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(axis, up)) > 0.999f)
    {
        up = Vector3(1.0f, 0.0f, 0.0f);
    }
    Vector3 right = glm::normalize(glm::cross(up, axis));
    up = glm::cross(axis, right);

    Matrix3 rotationMatrix(right, axis, up);
    return glm::quat_cast(rotationMatrix);
}

// Function to rotate the inertia tensor to match the capsule's orientation
Matrix3 rotateInertiaTensor(const Matrix3 &inertiaTensor, const Quat &rotation)
{
    Matrix3 rotationMatrix = glm::toMat3(rotation);
    return rotationMatrix * inertiaTensor * glm::transpose(rotationMatrix);
}

ShapeCapsule::ShapeCapsule(const Vector3 &a, const Vector3 &b, float radius, float density)
{
    this->a = a;
    this->b = b;
    this->center = (a + b) * 0.5f;
    this->height = glm::length(b - a);
    this->radius = radius;

    float dRadius = this->radius * this->radius;
    float dHeight = this->height * this->height;
    float massHalfSphere = (2.0f * dRadius * this->radius * CONST_PI) / 3.0f * density;
    float massCilinder = this->height * dRadius * CONST_PI * density;
    mass = massHalfSphere * 2.0f + massCilinder;

    float IxxIzzCilinder = massCilinder * (dHeight / 12.0f + dRadius / 4.0f);
    float IyyCilinder = massCilinder * (dRadius / 2.0f);

    float IxxIzzHemispheres = 2.0f * massHalfSphere * ((2.0f * dRadius / 5.0f) + (dHeight / 2.0f) + (3.0f * this->height * this->radius / 8.0f));
    float IyyHemispheres = 2.0f * massHalfSphere * (2.0f * dRadius / 5.0f);

    float xz = IxxIzzCilinder + IxxIzzHemispheres;
    float y = IyyCilinder + IyyHemispheres;

    Matrix3 tempInertiaTensor(0.0f);
    tempInertiaTensor[0] = Vector3(xz, 0.0f, 0.0f);
    tempInertiaTensor[1] = Vector3(0.0f, y, 0.0f);
    tempInertiaTensor[2] = Vector3(0.0f, 0.0f, xz);

    // Compute the rotation quaternion
    Quat rotation = computeRotationQuaternion(a, b);

    // Rotate the inertia tensor to match the capsule's orientation
    inertia = rotateInertiaTensor(tempInertiaTensor, rotation);
}

ShapeCollisionType ShapeCapsule::getType()
{
    return ShapeCollisionType::Capsule;
}

AABB ShapeCapsule::getAABB(const Matrix4 &model)
{
    Vector3 pa = Vector3(model * Vector4(a, 1.0f));
    Vector3 pb = Vector3(model * Vector4(b, 1.0f));

    Vector3 min = Vector3(
        fminf(pa.x, pb.x) - radius,
        fminf(pa.y, pb.y) - radius,
        fminf(pa.z, pb.z) - radius);

    Vector3 max = Vector3(
        fmaxf(pa.x, pb.x) + radius,
        fmaxf(pa.y, pb.y) + radius,
        fmaxf(pa.z, pb.z) + radius);

    return AABB(min, max);
}

int ShapeCapsule::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    float s, t;
    Vector3 pOnLine, pOnCapsule;
    getClosestPointSegmentSegment(ray.a, ray.b, cache->capsule.a, cache->capsule.b, s, t, pOnLine, pOnCapsule);
    float radius = cache->capsule.radius;

    Vector3 normal = glm::normalize(ray.b - ray.a);
    Vector3 m = ray.a - pOnCapsule;
    float b = glm::dot(m, normal);
    float c = glm::dot(m, m) - radius * radius;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return 0;
    float discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return 0;

    // Ray now found to intersect sphere, compute both points of intersection
    float sqrtDiscr = sqrtf(discr);
    float t1 = -b - sqrtDiscr;
    float t2 = -b + sqrtDiscr;
    float length = glm::length(ray.b - ray.a);

    // If t is negative, ray started inside sphere
    if (t1 < 0.0f)
    {
        if (t2 <= length)
        {
            Vector3 point = ray.a + t2 * normal;
            newPoints[0] = PhysicsBodyPoint({nullptr, point, glm::normalize(ray.a - point), t2});
            return 1;
        }
    }
    else
    {
        int out = 0;
        if (t1 <= length)
        {
            Vector3 point = ray.a + t1 * normal;
            newPoints[0] = PhysicsBodyPoint({nullptr, point, glm::normalize(ray.a - point), t1});
            out++;
        }
        if (t2 <= length)
        {
            Vector3 point = ray.a + t2 * normal;
            newPoints[out] = PhysicsBodyPoint({nullptr, point, glm::normalize(ray.a - point), t2});
            out++;
        }
        return out;
    }

    return 0;
}