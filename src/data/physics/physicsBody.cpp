// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "physicsBody.h"
#include "physicsWorld.h"

PhysicsBody::PhysicsBody(PhysicsMotionType motionType, PhysicsForm *form, PhysicsWorld *world, Entity *entity, Vector3 initialPosition, Quat initialRotation)
{
    this->motionType = motionType;
    this->form = form;
    this->world = world;
    this->entity = entity;
    this->position = initialPosition;
    this->rotation = initialRotation;

    entity->setPosition(this->position / world->getSimScale());
    entity->setRotation(this->rotation);
}

void PhysicsBody::prepareForSimulation()
{
    this->position = entity->getPosition() * world->getSimScale();
    this->rotation = entity->getRotation();
}

void PhysicsBody::finishSimulation()
{
    entity->setPosition(this->position / world->getSimScale());
    entity->setRotation(this->rotation);
}

void PhysicsBody::processStep(float delta, Vector3 &gravity)
{
    if (bIsSleeping && !bIsEnabled)
        return;

    if (motionType == PhysicsMotionType::Static)
        return setAsleep();

    Matrix3 r = glm::toMat3(rotation);

    linearVelocity += (gravity * form->getGravityFactor() + form->getInvertedMass() * force) * delta;
    angularVelocity += (r * (form->getInvertedInertia() * glm::transpose(r) * torque)) * delta;

    linearVelocity -= linearVelocity * linearDamping * delta;
    angularVelocity -= angularVelocity * angularDamping * delta;

    force = Vector3(0.0f, 0.0f, 0.0f);
    torque = Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::applyStep(float delta)
{
    if (glm::length2(translationAccumulator) > 0.0000000001f)
    {
        // if (!constraints.empty())
        //     for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
        //         translationAccumulator = (*constraint)->processTranslation(translationAccumulator);

        position += translationAccumulator;
        translationAccumulator = Vector3(0.0f);
        bIsSleeping = false;
    }

    if (!bIsSleeping && bIsEnabled)
    {
        checkLimits();

        // if (!constraints.empty())
        //     for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
        //         (*constraint)->processMotion(motion);

        position += linearVelocity * delta;

        Vector3 angularVelocityDelta = angularVelocity * delta;
        float len = glm::length(angularVelocityDelta);
        if (len > 1.0e-6f)
        {
            rotation = glm::normalize(glm::angleAxis(len, angularVelocityDelta / len) * rotation);
        }

        float checkFactor = delta * world->getSimScale() * 0.1f;
        if (glm::length2(linearVelocity) < checkFactor && glm::length2(angularVelocity) < checkFactor)
        {
            sleepAccumulator += delta;
            if (sleepAccumulator > 0.8f)
                setAsleep();
        }
        else
            sleepAccumulator = 0.0f;
    }

    /*
        for (auto &body : bodyCollisionData)
        {
            body.reaccuredTimer += delta;
        }
        */
}

void PhysicsBody::translate(Vector3 v)
{
    lock.lock();
    translationAccumulator += v;
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    lock.unlock();
}

void PhysicsBody::addLinearVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    linearVelocity += velocity;
    lock.unlock();
}

void PhysicsBody::addAngularVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    angularVelocity += velocity;
    lock.unlock();
}

void PhysicsBody::checkLimits()
{
    if (glm::length(linearVelocity) > 40.0f)
        linearVelocity = glm::normalize(linearVelocity) * 40.0f;
}