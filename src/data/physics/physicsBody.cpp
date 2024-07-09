// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "physicsBody.h"
#include "physicsWorld.h"

PhysicsBody::PhysicsBody(
    PhysicsMotionType motionType,
    PhysicsForm *form,
    PhysicsWorld *world,
    Entity *entity,
    void *userData,
    Vector3 initialPosition,
    Quat initialRotation)
{
    this->motionType = motionType;
    this->form = form;
    this->world = world;
    this->entity = entity;
    this->userData = userData;
    this->position = initialPosition;
    this->rotation = initialRotation;

    entity->setPosition(this->position / world->getSimScale());
    entity->setRotation(this->rotation);
    updateCache();
}

PhysicsBody::~PhysicsBody()
{
    if (world)
        world->removeBody(this);
}

void PhysicsBody::destroy()
{
    delete this;
}

void PhysicsBody::prepareForSimulation()
{
    this->position = entity->getPosition() * world->getSimScale();
    this->rotation = entity->getRotation();
    updateAABB();
    updateCache();
}

void PhysicsBody::finishSimulation()
{
    entity->setPosition(this->position / world->getSimScale());
    entity->setRotation(this->rotation);
}

void PhysicsBody::processStep(float delta, Vector3 &gravity)
{
    if (bIsSleeping && !bIsEnabled)
        return;

    if (motionType == PhysicsMotionType::Static)
        return setAsleep();

    Matrix3 r = glm::toMat3(rotation);

    linearVelocity += (gravity * form->getGravityFactor() + form->getInvertedMass() * force) * delta;
    angularVelocity += (r * (form->getInvertedInertia() * glm::transpose(r) * torque)) * delta;

    linearVelocity -= linearVelocity * linearDamping * delta;
    angularVelocity -= angularVelocity * angularDamping * delta;

    force = Vector3(0.0f, 0.0f, 0.0f);
    torque = Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::applyStep(float delta)
{
    if (motionType == PhysicsMotionType::Static)
        return;

    if (glm::length2(translationAccumulator) > 0.0000000001f)
    {
        // if (!constraints.empty())
        //     for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
        //         translationAccumulator = (*constraint)->processTranslation(translationAccumulator);

        position += translationAccumulator;
        translationAccumulator = Vector3(0.0f);
        bIsSleeping = false;
    }

    if (!bIsSleeping && bIsEnabled)
    {
        checkLimits();

        // if (!constraints.empty())
        //     for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
        //         (*constraint)->processMotion(motion);

        position += linearVelocity * delta;

        Vector3 angularVelocityDelta = angularVelocity * delta;
        float len = glm::length(angularVelocityDelta);
        if (len > 1.0e-6f)
        {
            rotation = glm::normalize(glm::angleAxis(len, angularVelocityDelta / len) * rotation);
        }

        float checkFactor = delta * world->getSimScale() * 0.1f;
        if (glm::length2(linearVelocity) < checkFactor && glm::length2(angularVelocity) < checkFactor)
        {
            sleepAccumulator += delta;
            if (sleepAccumulator > 0.8f)
                setAsleep();
        }
        else
            sleepAccumulator = 0.0f;

        updateAABB();
        updateCache();
    }

    /*
        for (auto &body : bodyCollisionData)
        {
            body.reaccuredTimer += delta;
        }
        */
}

void PhysicsBody::translate(Vector3 v)
{
    lock.lock();
    translationAccumulator += v;
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    lock.unlock();
}

void PhysicsBody::addLinearVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    linearVelocity += velocity;
    lock.unlock();
}

void PhysicsBody::addAngularVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    angularVelocity += velocity;
    lock.unlock();
}

void PhysicsBody::updateCache()
{
    if (!cache)
    {
        cacheBodies = 1;
        cache = new PhysicsBodyCache[1];
        memset(cache, 0, sizeof(PhysicsBodyCache) * cacheBodies);
    }

    if (form->getType() == ShapeCollisionType::Sphere)
    {
        ShapeSphere *sphere = (ShapeSphere *)form->getSimpleShape();
        cache[0].sphere.center = position;
        cache[0].sphere.radius = sphere->getRadius();
        return;
    }

    if (form->getType() == ShapeCollisionType::Convex)
    {
        ShapeConvex *convex = (ShapeConvex *)form->getSimpleShape();
        Matrix4 m = glm::toMat4(rotation);
        Matrix4 transformation = glm::translate(Matrix4(1.0f), position) * m;
        if (!cache[0].convex.points)
        {
            cache[0].convex.points = new Vector3[convex->getVertexAmount()];
            cache[0].convex.amount = convex->getVertexAmount();
        }
        int count = cache[0].convex.amount;
        Vector3 *verticies = convex->getVerticies();

        for (int i = 0; i < count; i++)
            cache[0].convex.points[i] = Vector3(transformation * Vector4(verticies[i], 1.0f));
    }

    if (form->getType() == ShapeCollisionType::OBB)
    {
        ShapeOBB *OBBShape = (ShapeOBB *)form->getSimpleShape();
        Matrix4 m = glm::toMat4(rotation);
        Matrix4 transformation = glm::translate(Matrix4(1.0f), position) * m;

        float halfWidth = OBBShape->getHalfWidth();
        float halfHeight = OBBShape->getHalfHeight();
        float halfDepth = OBBShape->getHalfDepth();

        cache[0].OBB.center = position;

        cache[0].OBB.points[0] = Vector3(transformation * Vector4(halfWidth, halfHeight, halfDepth, 1.0f));
        cache[0].OBB.points[1] = Vector3(transformation * Vector4(halfWidth, halfHeight, -halfDepth, 1.0f));
        cache[0].OBB.points[2] = Vector3(transformation * Vector4(-halfWidth, halfHeight, halfDepth, 1.0f));
        cache[0].OBB.points[3] = Vector3(transformation * Vector4(-halfWidth, halfHeight, -halfDepth, 1.0f));
        cache[0].OBB.points[4] = Vector3(transformation * Vector4(halfWidth, -halfHeight, halfDepth, 1.0f));
        cache[0].OBB.points[5] = Vector3(transformation * Vector4(halfWidth, -halfHeight, -halfDepth, 1.0f));
        cache[0].OBB.points[6] = Vector3(transformation * Vector4(-halfWidth, -halfHeight, halfDepth, 1.0f));
        cache[0].OBB.points[7] = Vector3(transformation * Vector4(-halfWidth, -halfHeight, -halfDepth, 1.0f));

        cache[0].OBB.normals[0] = rotateNormal(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), rotation);
        cache[0].OBB.normals[1] = rotateNormal(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), rotation);
        cache[0].OBB.normals[2] = rotateNormal(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), rotation);
        cache[0].OBB.normals[3] = rotateNormal(glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), rotation);
        cache[0].OBB.normals[4] = rotateNormal(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), rotation);
        cache[0].OBB.normals[5] = rotateNormal(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), rotation);

        cache[0].OBB.axisX = Vector3(m * Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        cache[0].OBB.axisY = Vector3(m * Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        cache[0].OBB.axisZ = Vector3(m * Vector4(0.0f, 0.0f, 1.0f, 1.0f));

        cache[0].OBB.transformation = transformation;
        return;
    }

    if (form->getType() == ShapeCollisionType::Plain)
    {
        ShapePlain *plain = (ShapePlain *)form->getSimpleShape();
        Vector3 normal = rotation * plain->getNormal();
        cache[0].plain.normal = normal;
        cache[0].plain.distance = plain->getDistance() + glm::dot(position, normal);
        return;
    }
}

int PhysicsBody::castRay(const Segment &ray, PhysicsBodyPoint *newPoints)
{
    if (cache)
    {
        int count = form->castRay(ray, newPoints, cache);
        for (int i = 0; i < count; i++)
            newPoints[i].userData = userData;
        return count;
    }
    return 0;
}