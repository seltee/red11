// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "physicsUtils.h"
#include "utils/destroyable.h"
#include <vector>

class CollisionHandler : public Destroyable
{
public:
    EXPORT void triggerCollision(PhysicsBody *bodyA, PhysicsBody *bodyB, Vector3 &pointA, Vector3 &pointB);
    EXPORT void removeNotPersistedCollisions();
    EXPORT void notifyBodyRemoved(PhysicsBody *body);
    EXPORT void updateCollisionTimers(float delta);

    EXPORT virtual void collisionStarted(BodyCollisionData *data);
    EXPORT virtual void collisionPersisted(BodyCollisionData *data);
    EXPORT virtual void collisionEnded(BodyCollisionData *data);

protected:
    // Holds happened collisions information
    std::vector<BodyCollisionData> bodyCollisionData;
};