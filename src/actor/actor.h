// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "utils/destroyable.h"
#include "components/component.h"
#include "components/componentMesh.h"
#include "components/componentMeshGroup.h"
#include "components/componentText.h"
#include "components/componentLight.h"
#include "components/componentCamera.h"
#include "data/mesh.h"
#include "renderer/renderer.h"
#include "data/entity.h"
#include "data/physics/physicsWorld.h"
#include <string>
#include <list>

class Scene;

class Actor : public Entity, public Destroyable
{
public:
    EXPORT Actor(std::string &name);
    EXPORT virtual ~Actor();

    EXPORT void setActorName(std::string &name);
    EXPORT const std::string &getActorName();

    EXPORT void assignPhysicsWorld(PhysicsWorld *physicsWorld);

    template <class T, typename std::enable_if<std::is_base_of<Component, T>::value>::type * = nullptr>
    inline T *createComponent()
    {
        auto newComonent = new T();
        prepareNewComponent(newComonent);
        return newComonent;
    }

    EXPORT void removeComponents();
    EXPORT void removeComponent(Component *component);
    EXPORT void prepareNewComponent(Component *component);

    inline ComponentMesh *createComponentMesh(Mesh *mesh, Vector3 position = Vector3(0.0f), Vector3 rotation = Vector3(0.0f), Vector3 scale = Vector3(1.0f))
    {
        auto component = createComponent<ComponentMesh>();
        component->setPosition(position);
        component->setRotation(rotation);
        component->setScale(scale);
        component->setMesh(mesh);
        return component;
    }

    inline ComponentMeshGroup *createComponentMeshGroup(std::vector<MeshObject *> *list, Vector3 position = Vector3(0.0f), Vector3 rotation = Vector3(0.0f), Vector3 scale = Vector3(1.0f))
    {
        auto component = createComponent<ComponentMeshGroup>();
        component->setPosition(position);
        component->setRotation(rotation);
        component->setScale(scale);
        component->setMeshList(list);
        return component;
    }

    inline ComponentText *createComponentText(std::string text, Font *font, int size, Vector3 position = Vector3(0.0f), Vector3 rotation = Vector3(0.0f), Vector3 scale = Vector3(1.0f))
    {
        auto component = createComponent<ComponentText>();
        component->setFont(font);
        component->setText(text);
        component->setSize(size);
        component->setPosition(position);
        component->setRotation(rotation);
        component->setScale(scale);
        return component;
    }

    EXPORT void setParent(Actor *parent);

    EXPORT Scene *getScene();
    EXPORT void setScene(Scene *scene);

    EXPORT void onProcess(float delta);
    EXPORT void onRenderQueue(Renderer *renderer);

    EXPORT virtual void onSpawned();
    EXPORT virtual void onSubProcess(float delta);

protected:
    std::string name;
    std::list<Component *> components;

    bool bPhysicsNeedsToBeRebuild = false;
    Scene *scene = nullptr;

    Actor *parent = nullptr;

    PhysicsWorld *physicsWorld = nullptr;
};
