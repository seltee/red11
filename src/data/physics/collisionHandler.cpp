// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "collisionHandler.h"

void CollisionHandler::triggerCollision(PhysicsBody *bodyA, PhysicsBody *bodyB, Vector3 &pointA, Vector3 &pointB)
{
    if (bodyA == bodyB)
        return;

    if (bodyA < bodyB)
    {
        PhysicsBody *temp = bodyA;
        bodyA = bodyB;
        bodyB = temp;
    }

    for (auto it = bodyCollisionData.begin(); it != bodyCollisionData.end(); it++)
    {
        if (it->bodyA == bodyA && (*it).bodyB == bodyB)
        {
            it->pointA = pointA;
            it->pointB = pointB;
            it->reaccuredTimer = 0.0f;
            collisionPersisted(&(*it));
            return;
        }
    }
    bodyCollisionData.push_back(BodyCollisionData({bodyA, bodyB, pointA, pointB, 0.0f, 0.0f}));
    collisionStarted(&bodyCollisionData.back());
}

void CollisionHandler::removeNotPersistedCollisions()
{
    auto it = bodyCollisionData.begin();
    while (it != bodyCollisionData.end())
    {
        if (it->reaccuredTimer > 0.015f)
        {
            it = bodyCollisionData.erase(it);
            collisionEnded(&(*it));
        }
        else
            ++it;
    }
}

void CollisionHandler::updateCollisionTimers(float delta)
{
    for (auto &data : bodyCollisionData)
    {
        data.persistedTimer += delta;
        data.reaccuredTimer += delta;
    }
}

void CollisionHandler::notifyBodyRemoved(PhysicsBody *body)
{
    auto it = bodyCollisionData.begin();
    while (it != bodyCollisionData.end())
    {
        if (it->bodyA == body || it->bodyB == body)
        {
            collisionEnded(&(*it));
            it = bodyCollisionData.erase(it);
        }
        else
            ++it;
    }
}

void CollisionHandler::collisionStarted(BodyCollisionData *data)
{
}

void CollisionHandler::collisionPersisted(BodyCollisionData *data)
{
}

void CollisionHandler::collisionEnded(BodyCollisionData *data)
{
}

/*
void PhysicsBody::removeNotPersistedCollisions()
{
    auto it = bodyCollisionData.begin();
    while (it != bodyCollisionData.end())
    {
        if (it->reaccuredTimer > 0.015f)
        {
            it = bodyCollisionData.erase(it);
            // if (owner)
            //     owner->onCollideStopped((*it).foreignBody->getOwner());
        }
        else
            ++it;
    }
}

void PhysicsBody::notifyBodyRemoved(PhysicsBody *body)
{
    for (auto it = bodyCollisionData.begin(); it != bodyCollisionData.end(); it++)
    {
        if (it->foreignBody == body)
        {
            // if (owner)
            //     owner->onCollideStopped(it->foreignBody->getOwner());
            bodyCollisionData.erase(it);
            break;
        }
    }
}
*/