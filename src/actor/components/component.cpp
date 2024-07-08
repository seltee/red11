// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component.h"
#include "actor/actor.h"
#include "red11.h"
#include <utils/glm/gtx/matrix_decompose.hpp>

Component::Component()
{
}

Component::~Component()
{
    if (physicsBody)
        delete physicsBody;
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

void Component::assignPhysicsWorld(PhysicsWorld *physicsWorld)
{
    this->physicsWorld = physicsWorld;
}

void Component::enablePhysics(PhysicsMotionType motionType, PhysicsForm *physicsForm, void *userData)
{
    if (!physicsBody && physicsWorld)
    {
        Vector3 scale;
        Quat rotation;
        Vector3 position;
        Vector3 skew;
        Vector4 perspective;

        glm::decompose(*getModelMatrix(), scale, rotation, position, skew, perspective);

        physicsBody = physicsWorld->createPhysicsBody(motionType, physicsForm, this, userData, position, rotation);

        this->setTransformationParent(nullptr);
    }
}

void Component::disablePhysics()
{
    if (physicsBody)
    {
        delete physicsBody;
        physicsBody = nullptr;
        setParent(parent);
    }
}

void Component::onCreated()
{
}

void Component::onRenderQueue(Renderer *renderer)
{
}

void Component::onRenderDebug(Renderer *renderer)
{
    if (bRenderDebugPhysicsBody && physicsBody)
    {
        auto debugEntities = Red11::getDebugEntities();
        auto form = physicsBody->getForm();
        float simScale = physicsBody->getPhysicsWorld()->getSimScale();

        if (form->getType() == ShapeCollisionType::Sphere)
        {
            auto data = physicsBody->getCacheSphere(0);
            float radius = data->radius / simScale;
            Vector3 center = data->center / simScale;
            Matrix4 m;

            for (int i = 0; i < 16; i++)
            {
                float lA = (float)i / 16.0f * CONST_PI * 2.0f;
                float lB = (float)(i + 1) / 16.0f * CONST_PI * 2.0f;

                m = debugEntities->makeDebugCubeIntoLineMatrix(
                    center + Vector3(radius * sinf(lA), radius * cosf(lA), 0.0f),
                    center + Vector3(radius * sinf(lB), radius * cosf(lB), 0.0f));
                renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);

                m = debugEntities->makeDebugCubeIntoLineMatrix(
                    center + Vector3(0.0f, radius * sinf(lA), radius * cosf(lA)),
                    center + Vector3(0.0f, radius * sinf(lB), radius * cosf(lB)));
                renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);

                m = debugEntities->makeDebugCubeIntoLineMatrix(
                    center + Vector3(radius * cosf(lA), 0.0f, radius * sinf(lA)),
                    center + Vector3(radius * cosf(lB), 0.0f, radius * sinf(lB)));
                renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            }
            return;
        }

        if (form->getType() == ShapeCollisionType::Plain)
        {
            auto data = physicsBody->getCachePlain(0);
            Vector3 center = data->normal * -data->distance / simScale;
            Vector3 side = glm::cross(data->normal, Vector3(0, 1.0f, 0));
            Vector3 topSide = glm::cross(data->normal, side);
            Matrix4 m1 = debugEntities->makeDebugCubeIntoLineMatrix(center + side * -2.0f, center + side * 2.0f);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m1);
            Matrix4 m2 = debugEntities->makeDebugCubeIntoLineMatrix(center + topSide * -2.0f, center + topSide * 2.0f);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m2);
            return;
        }
    }
}

void Component::onProcess(float delta)
{
}

void Component::setParent(Component *parent)
{
    if (parent && parent->getOwner() == owner)
    {
        this->parent = parent;
        if (!physicsBody)
            this->setTransformationParent(parent);
    }
    else
    {
        this->parent = nullptr;
        if (!physicsBody)
            this->setTransformationParent(owner);
    }
}
