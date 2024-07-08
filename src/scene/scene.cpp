// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "scene.h"

Scene::Scene(DebugEntities *debugEntities)
{
    this->debugEntities = debugEntities;
}

void Scene::prepareNewActor(Actor *actor)
{
    actors.push_back(actor);
    actor->assignPhysicsWorld(&physicsWorld);
    actor->setScene(this);
    actor->onSpawned();
}

void Scene::process(float delta)
{
    physicsWorld.process(delta);
    for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        (*actor)->onProcess(delta);

    auto actor = actors.begin();
    while (actor != actors.end())
        if ((*actor)->isDestroyed())
        {
            delete (*actor);
            actor = actors.erase(actor);
        }
        else
            ++actor;
}

void Scene::render(Renderer *renderer, Camera *camera)
{
    renderer->setAmbientLight(ambientLight);

    for (auto &actor : actors)
        actor->onRenderQueue(renderer);

    renderer->renderQueue(camera);
    renderer->clearQueue();
}
