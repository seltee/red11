// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component.h"
#include "actor/actor.h"

Component::Component()
{
}

Component::~Component()
{
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
        this->setTransformationParent(parent);
    }
    else
    {
        this->parent = nullptr;
        this->setTransformationParent(owner);
    }
}
