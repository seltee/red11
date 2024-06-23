// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "scene.h"

void Scene::prepareNewActor(Actor *actor)
{
    // if (physicsWorld)
    //     actor->setPhysicsWorld(physicsWorld);

    actors.push_back(actor);
    actor->setScene(this);
    actor->onSpawned();
    // actor->getPhysicsBody();
}

void Scene::process(float delta)
{
    for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        (*actor)->onProcess(delta);
}

void Scene::render(Renderer *renderer, Camera *camera)
{
    renderer->setAmbientLight(ambientLight);

    for (auto &actor : actors)
        actor->onRenderQueue(renderer);

    renderer->renderQueue(camera);
    renderer->clearQueue();
}
