// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "actor/actor.h"
#include "renderer/renderer.h"
#include "data/camera.h"
#include "data/physics/physicsWorld.h"
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

    inline std::vector<PhysicsBodyPoint> castRayCollision(const Segment &ray)
    {
        return physicsWorld.castRayCollision(ray);
    }

    inline std::vector<PhysicsBodyPoint> castRayCollision(const Vector3 &from, const Vector3 &to)
    {
        Segment ray = Segment(from, to);
        return physicsWorld.castRayCollision(ray);
    }

    inline std::vector<PhysicsBodyPoint> castSphereCollision(const Vector3 &p, float radius)
    {
        return physicsWorld.castSphereCollision(p, radius);
    }

    inline std::vector<PhysicsBodyPoint> castPointCollision(const Vector3 &p)
    {
        return physicsWorld.castPointCollision(p);
    }

    inline void setAmbientLight(Color color) { this->ambientLight = color; }
    inline Color getAmbientLight() { return ambientLight; };

    inline PhysicsWorld *getPhysicsWorld() { return &physicsWorld; }

protected:
    std::list<Actor *> actors;
    Color ambientLight = Color(0.4f, 0.4f, 0.44f);
    PhysicsWorld physicsWorld;
};