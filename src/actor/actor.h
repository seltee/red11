// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "utils/destroyable.h"
#include "components/component.h"
#include "components/componentMesh.h"
#include "components/componentMeshGroup.h"
#include "components/componentSpline.h"
#include "components/componentText.h"
#include "components/componentLight.h"
#include "components/componentCamera.h"
#include "data/mesh.h"
#include "data/entity.h"
#include "physics/physicsWorld.h"
#include "renderer/renderer.h"
#include <string>
#include <list>

class Scene;

class Actor : public Entity, public Destroyable
{
public:
    EXPORT Actor(const std::string &name);
    EXPORT Actor();
    EXPORT virtual ~Actor();

    EXPORT void setActorName(const std::string &name);
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

    inline ComponentMesh *createComponentMesh(Mesh *mesh, const Vector3 position = Vector3(0.0f), const Vector3 rotation = Vector3(0.0f), const Vector3 scale = Vector3(1.0f))
    {
        auto component = createComponent<ComponentMesh>();
        component->setPosition(position);
        component->setRotation(rotation);
        component->setScale(scale);
        component->setMesh(mesh);
        return component;
    }

    inline ComponentMeshGroup *createComponentMeshGroup(std::vector<MeshObject *> *list, const Vector3 position = Vector3(0.0f), const Vector3 rotation = Vector3(0.0f), const Vector3 scale = Vector3(1.0f))
    {
        auto component = createComponent<ComponentMeshGroup>();
        component->setPosition(position);
        component->setRotation(rotation);
        component->setScale(scale);
        component->setMeshList(list);
        return component;
    }

    inline ComponentText *createComponentText(std::string text, Font *font, int size, const Vector3 position = Vector3(0.0f), const Vector3 rotation = Vector3(0.0f), const Vector3 scale = Vector3(1.0f))
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

    inline ComponentSpline *createComponentSpline(Spline *spline, const Vector3 position = Vector3(0.0f), const Vector3 rotation = Vector3(0.0f), const Vector3 scale = Vector3(1.0f))
    {
        auto component = createComponent<ComponentSpline>();
        component->setPosition(position);
        component->setRotation(rotation);
        component->setScale(scale);
        component->setSpline(spline);
        return component;
    }

    inline void setVisibility(bool state) { bVisible = state; }
    inline bool getVisibility() { return bVisible; }

    EXPORT void setParent(Actor *parent);

    EXPORT Scene *getScene();
    EXPORT void setScene(Scene *scene);

    EXPORT void process(float fDelta);
    EXPORT void renderQueue(Renderer *renderer);

    EXPORT virtual void onSpawned();
    EXPORT virtual void onProcess(float fDelta);

protected:
    std::string name;
    std::list<Component *> components;

    bool bPhysicsNeedsToBeRebuild = false;
    Scene *scene = nullptr;

    Actor *parent = nullptr;

    PhysicsWorld *physicsWorld = nullptr;

    bool bVisible = true;
};
