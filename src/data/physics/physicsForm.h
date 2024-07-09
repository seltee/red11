// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "shapes/shape.h"
#include "shapes/shapePlain.h"
#include "shapes/shapeSphere.h"
#include "shapes/shapeOBB.h"
#include "shapes/shapeConvex.h"
#include "physicsUtils.h"
#include <vector>

class PhysicsForm
{
public:
    EXPORT PhysicsForm(float simScale, float friction, float restitution, float linearDamping, float angularDamping, float gravityFactor);
    EXPORT ~PhysicsForm();
    EXPORT void destroy();

    EXPORT ShapePlain *createPlain(Vector3 normal, float distance);
    EXPORT ShapePlain *createPlain(Vector3 normal, Vector3 point);
    EXPORT ShapeSphere *createSphere(Vector3 center, float radius, float density = 22.0f);
    EXPORT ShapeOBB *createOBB(Vector3 center, float width, float height, float depth, float density = 22.0f);
    EXPORT ShapeOBB *createOBB(Vector3 center, float size, float density = 22.0f);

    EXPORT void recalcParameters();

    EXPORT AABB getAABB(Matrix4 *model);

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache);

    inline Matrix3 &getInvertedInertia() { return invertedInteria; }

    inline float getMass() { return mass; }
    inline void setMass(float mass) { this->mass = mass; }

    inline float getFriction() { return friction; }
    inline float getRestitution() { return restitution; }
    inline float getLinearDamping() { return linearDamping; }
    inline float getAngluarDamping() { return angularDamping; }
    inline float getGravityFactor() { return gravityFactor; }

    inline float getInvertedMass() { return invertedMass; }

    inline ShapeCollisionType getType() { return type; }

    inline Shape *getSimpleShape() { return shapes.at(0); }

protected:
    std::vector<Shape *> shapes;
    float simScale;
    bool isDataDirty = false;

    // Shape data
    float friction;
    float restitution;
    float linearDamping;
    float angularDamping;
    float gravityFactor;

    // Mass and inverted mass
    float mass = 0.1f;
    float invertedMass;

    // Intertia
    Matrix3 invertedInteria;

    // Type of the shape
    ShapeCollisionType type;
};