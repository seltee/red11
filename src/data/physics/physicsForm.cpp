// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "physicsForm.h"

PhysicsForm::PhysicsForm(float simScale, float friction, float restitution, float linearDamping, float angularDamping, float gravityFactor)
{
    this->simScale = simScale;
    this->friction = friction;
    this->restitution = restitution;
    this->linearDamping = linearDamping;
    this->angularDamping = angularDamping;
    this->gravityFactor = gravityFactor;

    invertedInteria = Matrix3(1.0f);
    invertedMass = 1.0f / mass;
}

PhysicsForm::~PhysicsForm()
{
}

void PhysicsForm::destroy()
{
}

ShapePlain *PhysicsForm::createPlain(Vector3 normal, float distance)
{
    ShapePlain *newPlain = new ShapePlain(normal, distance * simScale);
    shapes.push_back(newPlain);
    isDataDirty = true;
    recalcParameters();
    return newPlain;
}

ShapePlain *PhysicsForm::createPlain(Vector3 normal, Vector3 point)
{
    ShapePlain *newPlain = new ShapePlain(normal, point * simScale);
    shapes.push_back(newPlain);
    isDataDirty = true;
    recalcParameters();
    return newPlain;
}

ShapeSphere *PhysicsForm::createSphere(Vector3 position, float radius, float density)
{
    ShapeSphere *newPlain = new ShapeSphere(position * simScale, radius * simScale, density);
    shapes.push_back(newPlain);
    isDataDirty = true;
    recalcParameters();
    return newPlain;
}

void PhysicsForm::recalcParameters()
{
    if (isDataDirty)
    {
        isDataDirty = false;

        if (shapes.size() == 0)
        {
            type = ShapeCollisionType::None;
            invertedInteria = Matrix3(1.0f);
            mass = 1.0f;
        }
        if (shapes.size() == 1)
        {
            type = shapes.at(0)->getType();
            invertedInteria = glm::inverse(shapes.at(0)->getInertiaTensor());
            mass = shapes.at(0)->getMass();
        }
        if (shapes.size() > 1)
        {
            type = ShapeCollisionType::Combined;
            invertedInteria = glm::inverse(shapes.at(0)->getInertiaTensor());
            mass = 0.0f;
            for (auto &shape : shapes)
                mass += shape->getMass();
        }

        invertedMass = 1.0f / mass;
    }
}

AABB PhysicsForm::getAABB(Matrix4 *model)
{
    if (shapes.size() == 1)
    {
        return shapes.at(0)->getAABB(model);
    }
    if (shapes.size() > 1)
    {
        AABB aabb = shapes.at(0)->getAABB(model);
        for (int i = 1; i < shapes.size(); i++)
        {
            aabb.extend(shapes.at(i)->getAABB(model));
        }
        return aabb;
    }
    Vector3 p = Vector3(*model * Vector4(0, 0, 0, 1.0f));
    return AABB(p, p);
}