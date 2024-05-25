// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/destroyable.h"
#include "renderer/renderer.h"
#include "data/entity.h"

class Actor;

class Component : public Entity, public Destroyable
{
public:
    EXPORT Component();
    EXPORT virtual ~Component();
    EXPORT void prepare(Actor *owner);
    EXPORT Actor *getOwner();

    EXPORT virtual void onCreated();
    EXPORT virtual void onRender(Renderer *renderer);
    EXPORT virtual void onRenderQueue(Renderer *renderer);
    EXPORT virtual void onProcess(float delta);

    EXPORT void setParent(Component *parent);

protected:
    Actor *owner = nullptr;
    Component *parent = nullptr;
};