// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "actor/actor.h"
#include "actor/actorTemporary.h"
#include "renderer/renderer.h"
#include "data/camera.h"
#include "data/physics/physicsWorld.h"
#include "data/debugEntities.h"
#include <string>

class Scene
{
public:
    Scene(DebugEntities *debugEntities);

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

    inline ActorTemporary *createTemporaryActor(float timeToExist)
    {
        ActorTemporary *actor = createActor<ActorTemporary>("DebugActor");
        actor->setTimeToExist(timeToExist);
        return actor;
    }

    inline std::vector<PhysicsBodyPoint> castRayCollision(const Segment &ray, bool debug = false, float debugTimeSeconds = 3.0f)
    {
        if (debug)
        {
            auto points = physicsWorld.castRayCollision(ray);
            ActorTemporary *debugActor = createTemporaryActor(debugTimeSeconds);
            if (points.size() > 0)
            {
                auto lineCubeMeshA = debugActor->createComponentMesh(debugEntities->debugCubeMesh);
                lineCubeMeshA->setMaterial(debugEntities->matPositive);
                debugEntities->makeDebugCubeIntoLine(lineCubeMeshA, ray.a, points.at(0).point);
                auto lineCubeMeshB = debugActor->createComponentMesh(debugEntities->debugCubeMesh);
                lineCubeMeshB->setMaterial(debugEntities->matNegative);
                debugEntities->makeDebugCubeIntoLine(lineCubeMeshB, points.at(0).point, ray.b);

                for (auto &point : points)
                {
                    auto pointCubeMesh = debugActor->createComponentMesh(debugEntities->debugCubeMesh);
                    pointCubeMesh->setMaterial(debugEntities->matNegative);
                    debugEntities->makeDebugCubeIntoPoint(pointCubeMesh, point.point, &ray.b);
                }
            }
            else
            {
                auto lineCubeMesh = debugActor->createComponentMesh(debugEntities->debugCubeMesh);
                lineCubeMesh->setMaterial(debugEntities->matPositive);
                debugEntities->makeDebugCubeIntoLine(lineCubeMesh, ray.a, ray.b);
            }
            return points;
        }
        else
        {
            return physicsWorld.castRayCollision(ray);
        }
    }

    inline std::vector<PhysicsBodyPoint> castRayCollision(const Vector3 &from, const Vector3 &to, bool debug = false, float debugTimeSeconds = 3.0f)
    {
        Segment ray = Segment(from, to);
        return castRayCollision(ray, debug, debugTimeSeconds);
    }

    inline std::vector<PhysicsBodyPoint> castSphereCollision(const Vector3 &p, float radius, bool debug = false, float debugTimeSeconds = 3.0f)
    {
        return physicsWorld.castSphereCollision(p, radius);
    }

    inline std::vector<PhysicsBodyPoint> castPointCollision(const Vector3 &p, bool debug = false, float debugTimeSeconds = 3.0f)
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
    DebugEntities *debugEntities;
};