// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component.h"
#include "actor/actor.h"

Component::Component()
{
}

Component::~Component()
{
    if (physicsBody)
        delete physicsBody;
}

void Component::prepare(Actor *owner)
{
    this->owner = owner;
    this->setTransformationParent(owner);
    this->onCreated();
}

Actor *Component::getOwner()
{
    return owner;
}

void Component::assignPhysicsWorld(PhysicsWorld *physicsWorld)
{
    this->physicsWorld = physicsWorld;
}

void Component::enablePhysics(PhysicsMotionType motionType, PhysicsForm *physicsForm)
{
    if (!physicsBody && physicsWorld)
    {
        Vector3 position = *getModelMatrix() * Vector4(0, 0, 0, 1.0f);
        Quat rotation = glm::quat_cast(*getModelMatrix());

        physicsBody = physicsWorld->createPhysicsBody(motionType, physicsForm, this, position, rotation);

        this->setTransformationParent(nullptr);
    }
}

void Component::disablePhysics()
{
    if (physicsBody)
    {
        delete physicsBody;
        physicsBody = nullptr;
        setParent(parent);
    }
}

void Component::onCreated()
{
}

void Component::onRenderQueue(Renderer *renderer)
{
}

void Component::onProcess(float delta)
{
}

void Component::setParent(Component *parent)
{
    if (parent && parent->getOwner() == owner)
    {
        this->parent = parent;
        if (!physicsBody)
            this->setTransformationParent(parent);
    }
    else
    {
        this->parent = nullptr;
        if (!physicsBody)
            this->setTransformationParent(owner);
    }
}
