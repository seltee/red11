// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/destroyable.h"
#include "renderer/renderer.h"
#include "data/entity.h"
#include "data/physics/physicsWorld.h"

class Actor;

class Component : public Entity, public Destroyable
{
public:
    EXPORT Component();
    EXPORT virtual ~Component();
    EXPORT void prepare(Actor *owner);
    EXPORT Actor *getOwner();
    EXPORT void setCollisionHandler(CollisionHandler *collisionHandler);
    EXPORT CollisionHandler *getCollisionHandler();

    EXPORT void assignPhysicsWorld(PhysicsWorld *physicsWorld);
    EXPORT void enableCollisions(PhysicsMotionType motionType, PhysicsForm *physicsForm, void *userData = nullptr, bool simulatePhysics = true, Channel channel = CHANNELS_DEAFULT);
    EXPORT void disableCollisions();
    inline PhysicsBody *getPhysicsBody() { return physicsBody; }

    EXPORT virtual void onCreated();
    EXPORT virtual void onRenderQueue(Renderer *renderer);
    EXPORT virtual void onRenderDebug(Renderer *renderer);
    EXPORT virtual void onProcess(float delta);

    EXPORT void setParent(Component *parent);

    inline void addConstraint(Constraint *constraint)
    {
        if (physicsBody)
            physicsBody->addConstraint(constraint);
    }

    inline void removeConstraint(Constraint *constraint)
    {
        if (physicsBody)
            physicsBody->removeConstraint(constraint);
    }

    inline void setRenderDebugPhysicsBody(bool bRenderDebugPhysicsBody) { this->bRenderDebugPhysicsBody = bRenderDebugPhysicsBody; }
    inline bool getRenderDebugPhysicsBody() { return bRenderDebugPhysicsBody; }
    inline bool hasDebugInfoToRender() { return bRenderDebugPhysicsBody; }

    inline void setVisibility(bool state) { bVisible = state; }
    inline bool getVisibility() { return bVisible; }

protected:
    Actor *owner = nullptr;
    Component *parent = nullptr;
    PhysicsWorld *physicsWorld = nullptr;
    PhysicsBody *physicsBody = nullptr;
    bool bRenderDebugPhysicsBody = false;
    bool bVisible = true;
};