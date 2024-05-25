// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "actor/actor.h"
#include "renderer/renderer.h"
#include "data/camera.h"
#include <string>

class Scene
{
public:
    template <class T, typename std::enable_if<std::is_base_of<Actor, T>::value>::type * = nullptr>
    EXPORT T *createActor(std::string name)
    {
        auto newActor = new T(name);
        prepareNewActor(newActor);
        return newActor;
    }

    EXPORT void prepareNewActor(Actor *actor);

    EXPORT void process(float delta);
    EXPORT void render(Renderer *renderer, Camera *camera);

    inline void setAmbientLight(Color color) { this->ambientLight = color; }
    inline Color getAmbientLight() { return ambientLight; };

protected:
    std::list<Actor *> actors;
    Color ambientLight = Color(0.4f, 0.4f, 0.44f);
};