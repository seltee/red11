// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "actor.h"
#include "scene/scene.h"

Actor::Actor(std::string &name)
{
    this->name = name;
}

Actor::~Actor()
{
    removeComponents();
}

void Actor::setActorName(std::string &name)
{
    this->name = name;
}

const std::string &Actor::getActorName()
{
    return this->name;
}

void Actor::assignPhysicsWorld(PhysicsWorld *physicsWorld)
{
    this->physicsWorld = physicsWorld;
    for (auto &it : components)
    {
        it->assignPhysicsWorld(physicsWorld);
    }
}

void Actor::removeComponents()
{
    for (auto it = components.begin(); it != components.end(); it++)
        delete (*it);
    components.clear();
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::removeComponent(Component *component)
{
    for (auto it = components.begin(); it != components.end(); it++)
    {
        if (*it == component)
            delete *it;
        bPhysicsNeedsToBeRebuild = true;
        return;
    }
}

void Actor::prepareNewComponent(Component *component)
{
    this->components.push_back(component);
    component->prepare(this);
    component->assignPhysicsWorld(physicsWorld);
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::setParent(Actor *parent)
{
    this->parent = parent;
    setTransformationParent(parent);
    bIsTransformationDirty = true;
}

Scene *Actor::getScene()
{
    return scene;
}

void Actor::setScene(Scene *scene)
{
    this->scene = scene;
}

void Actor::onProcess(float delta)
{
    onSubProcess(delta);

    if (!components.empty())
    {
        auto it = components.begin();
        while (it != components.end())
        {
            (*it)->onProcess(delta);

            if ((*it)->isDestroyed())
            {
                delete (*it);
                it = components.erase(it);
                bPhysicsNeedsToBeRebuild = true;
            }
            else
                ++it;
        }
    }
}

void Actor::onRenderQueue(Renderer *renderer)
{
    if (!components.empty() && bVisible)
    {
        for (auto &component : components)
        {
            component->onRenderQueue(renderer);
            if (component->hasDebugInfoToRender())
                component->onRenderDebug(renderer);
        }
    }
}

void Actor::onSpawned()
{
}

void Actor::onSubProcess(float delta)
{
}