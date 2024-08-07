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

void Component::setCollisionHandler(CollisionHandler *collisionHandler)
{
    if (this->physicsBody)
    {
        this->physicsBody->setCollisionHandler(collisionHandler);
    }
}

CollisionHandler *Component::getCollisionHandler()
{
    if (this->physicsBody)
    {
        return this->physicsBody->getCollisionHandler();
    }
    return nullptr;
}

void Component::assignPhysicsWorld(PhysicsWorld *physicsWorld)
{
    this->physicsWorld = physicsWorld;
}

void Component::enableCollisions(PhysicsMotionType motionType, PhysicsForm *physicsForm, void *userData, bool simulatePhysics, Channel channel)
{
    if (physicsWorld)
    {
        if (!physicsBody)
        {
            if (!simulatePhysics)
            {
                physicsBody = physicsWorld->createPhysicsBody(motionType, physicsForm, this, getPosition(), getRotation(), simulatePhysics);
            }
            else
            {
                Vector3 scale;
                Quat rotation;
                Vector3 position;
                Vector3 skew;
                Vector4 perspective;

                glm::decompose(*getModelMatrix(), scale, rotation, position, skew, perspective);

                physicsBody = physicsWorld->createPhysicsBody(motionType, physicsForm, this, position, rotation, simulatePhysics);

                this->setTransformationParent(nullptr);
            }
        }

        if (physicsBody)
        {
            physicsBody->setUserData(userData);
            physicsBody->setChannel(channel);
        }
    }
}

void Component::disableCollisions()
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

        if (form->getType() == ShapeCollisionType::Capsule)
        {
            Matrix4 m1;
            auto data = physicsBody->getCacheCapsule(0);
            float radius = data->radius / simScale;
            Vector3 pa = data->a / simScale;
            Vector3 pb = data->b / simScale;

            Vector3 up = glm::length(pb - pa) == 0.0f ? Vector3(0.0f, 1.0f, 0.0f) : glm::normalize(pb - pa);
            Vector3 right = (up == Vector3(0.0f, 1.0f, 0.0f) || up == Vector3(0.0f, -1.0f, 0.0f)) ? Vector3(1.0f, 0.0f, 0.0f) : glm::normalize(glm::cross(up, Vector3(0.0f, 1.0f, 0.0f)));
            Vector3 forward = glm::normalize(glm::cross(right, up));

            Matrix4 transform = Matrix4(1.0f);
            transform[0] = Vector4(right, 0.0f);
            transform[1] = Vector4(up, 0.0f);
            transform[2] = Vector4(forward, 0.0f);

            for (int i = 0; i < 16; ++i)
            {
                float thetaFrom = CONST_PI2 * static_cast<float>(i) / 16.0f;
                float cosThetaFrom = cos(thetaFrom);
                float sinThetaFrom = sin(thetaFrom);
                float thetaTo = CONST_PI2 * static_cast<float>(i + 1) / 16.0f;
                float cosThetaTo = cos(thetaTo);
                float sinThetaTo = sin(thetaTo);

                Vector3 offsetFrom = radius * (cosThetaFrom * right + sinThetaFrom * forward);
                Vector3 offsetTo = radius * (cosThetaTo * right + sinThetaTo * forward);

                m1 = debugEntities->makeDebugCubeIntoLineMatrix(pa + offsetFrom, pa + offsetTo);
                renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m1);
                m1 = debugEntities->makeDebugCubeIntoLineMatrix(pb + offsetFrom, pb + offsetTo);
                renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m1);

                if ((i % 4) == 0)
                {
                    m1 = debugEntities->makeDebugCubeIntoLineMatrix(pa + offsetFrom, pb + offsetFrom);
                    renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m1);
                }
            }

            auto generateHemisphere = [&](const Vector3 &center, const Vector3 &direction)
            {
                Vector3 localUp = direction;
                Vector3 localRight = (direction == Vector3(0.0f, 1.0f, 0.0f) || direction == Vector3(0.0f, -1.0f, 0.0f)) ? Vector3(1.0f, 0.0f, 0.0f) : glm::normalize(glm::cross(localUp, Vector3(0.0f, 1.0f, 0.0f)));
                Vector3 localForward = glm::normalize(glm::cross(localRight, localUp));

                for (int j = 0; j <= 16; j++)
                {
                    float thetaFrom = CONST_PI * 0.5f - CONST_PI * static_cast<float>(j) / 16.0f;
                    float cosThetaFrom = cos(thetaFrom);
                    float sinThetaFrom = sin(thetaFrom);
                    float thetaTo = CONST_PI * 0.5f - CONST_PI * static_cast<float>(j + 1) / 16.0f;
                    float cosThetaTo = cos(thetaTo);
                    float sinThetaTo = sin(thetaTo);

                    Vector3 localPosFromA = radius * (sinThetaFrom * localRight + cosThetaFrom * localUp);
                    Vector3 localPosToA = radius * (sinThetaTo * localRight + cosThetaTo * localUp);
                    Vector3 localPosFromB = radius * (sinThetaFrom * localForward + cosThetaFrom * localUp);
                    Vector3 localPosToB = radius * (sinThetaTo * localForward + cosThetaTo * localUp);

                    m1 = debugEntities->makeDebugCubeIntoLineMatrix(center + localPosFromA, center + localPosToA);
                    renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m1);
                    m1 = debugEntities->makeDebugCubeIntoLineMatrix(center + localPosFromB, center + localPosToB);
                    renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m1);
                }
            };

            generateHemisphere(pb, up);
            generateHemisphere(pa, -up);
        }

        if (form->getType() == ShapeCollisionType::OBB)
        {
            auto data = physicsBody->getCacheOBB(0);
            Matrix4 m;
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[0] / simScale, data->points[1] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[1] / simScale, data->points[3] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[0] / simScale, data->points[2] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[2] / simScale, data->points[3] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[4] / simScale, data->points[5] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[5] / simScale, data->points[7] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[4] / simScale, data->points[6] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[6] / simScale, data->points[7] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[0] / simScale, data->points[4] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[1] / simScale, data->points[5] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[2] / simScale, data->points[6] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);
            m = debugEntities->makeDebugCubeIntoLineMatrix(data->points[3] / simScale, data->points[7] / simScale);
            renderer->queueMesh(debugEntities->debugCubeMesh, debugEntities->matPhysics, m);

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
