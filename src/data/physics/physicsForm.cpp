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

ShapeSphere *PhysicsForm::createSphere(Vector3 position, float radius)
{
    ShapeSphere *newPlain = new ShapeSphere(position * simScale, radius * simScale);
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