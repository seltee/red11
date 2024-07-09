// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/math.h"
#include "data/physics/physicsUtils.h"
#include <string>
#include <vector>

enum class ShapeCollisionType
{
    None = 0,
    Plain = 1,
    Sphere = 2,
    Convex = 3,
    OBB = 4,
    Geometry = 5,
    Capsule = 6,
    Combined = 7,
    Amount = 8
};

class Shape
{
protected:
    Shape();

public:
    virtual ~Shape();
    EXPORT virtual ShapeCollisionType getType() = 0;
    EXPORT virtual AABB getAABB(Matrix4 *model) = 0;
    EXPORT virtual int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) = 0;
    inline void setMass(float mass) { this->mass = mass; }
    inline float getMass() { return mass; }
    inline void setDebugName(std::string str) { name = str; }
    inline Matrix3 &getInertiaTensor() { return inertia; }

    EXPORT static std::string getTypeName(ShapeCollisionType type);

protected:
    float mass;
    std::string name;
    Matrix3 inertia;
};
