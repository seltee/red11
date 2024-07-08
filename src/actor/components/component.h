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

    EXPORT void assignPhysicsWorld(PhysicsWorld *physicsWorld);
    EXPORT void enablePhysics(PhysicsMotionType motionType, PhysicsForm *physicsForm, void *userData = nullptr);
    EXPORT void disablePhysics();
    inline PhysicsBody *getPhysicsBody() { return physicsBody; }

    EXPORT virtual void onCreated();
    EXPORT virtual void onRenderQueue(Renderer *renderer);
    EXPORT virtual void onRenderDebug(Renderer *renderer);
    EXPORT virtual void onProcess(float delta);

    EXPORT void setParent(Component *parent);

    inline void setRenderDebugPhysicsBody(bool bRenderDebugPhysicsBody) { this->bRenderDebugPhysicsBody = bRenderDebugPhysicsBody; }
    inline bool getRenderDebugPhysicsBody() { return bRenderDebugPhysicsBody; }
    inline bool hasDebugInfoToRender() { return bRenderDebugPhysicsBody; }

protected:
    Actor *owner = nullptr;
    Component *parent = nullptr;
    PhysicsWorld *physicsWorld = nullptr;
    PhysicsBody *physicsBody = nullptr;
    bool bRenderDebugPhysicsBody = false;
};