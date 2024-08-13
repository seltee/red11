// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/entity.h"
#include "utils/destroyable.h"
#include "utils/utils.h"
#include "utils/primitives.h"
#include "constraints/constraint.h"
#include "physicsForm.h"
#include "collisionHandler.h"
#include "channels.h"
#include <mutex>

class PhysicsWorld;

enum class PhysicsMotionType
{
    Static,  // Non movable
    Dynamic, // Responds to forces as a normal physics object
};

class PhysicsBody : public Destroyable
{
public:
    EXPORT PhysicsBody(
        PhysicsMotionType motionType,
        PhysicsForm *form,
        PhysicsWorld *world,
        Entity *entity,
        Vector3 initialPosition,
        Quat initialRotation,
        bool simulatePhysics);
    EXPORT virtual ~PhysicsBody();

    EXPORT void prepareForSimulation();
    EXPORT void finishSimulation();
    EXPORT void processStep(float delta, Vector3 &gravity);
    EXPORT void applyStep(float delta);

    EXPORT void translate(Vector3 v);
    EXPORT void addLinearVelocity(Vector3 velocity);
    EXPORT void addAngularVelocity(Vector3 velocity);

    EXPORT void updateCache();

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints);

    EXPORT void addConstraint(Constraint *constraint);
    EXPORT void removeConstraint(Constraint *constraint);
    EXPORT std::vector<Constraint *> *getConstraints();

    inline void setUserData(void *userData) { this->userData = userData; }
    inline void *getUserData() { return userData; }

    inline void setChannel(Channel channel) { this->channel = channel; }
    inline Channel getChannel() { return this->channel; }

    inline void setCollisionHandler(CollisionHandler *collisionHandler) { this->collisionHandler = collisionHandler; }
    inline CollisionHandler *getCollisionHandler() { return this->collisionHandler; }

    inline Vector3 getPointVelocity(const Vector3 &localPoint) { return linearVelocity + glm::cross(angularVelocity, localPoint); }
    inline const Vector3 &getLinearVelocity() { return linearVelocity; }
    inline const Vector3 &getAngularVelocity() { return angularVelocity; }

    inline bool isSleeping() { return bIsSleeping; }
    inline void forceWake() { bIsSleeping = false; }
    inline bool isEnabled() { return this->bIsEnabled; }
    inline void setEnabled(bool bState) { this->bIsEnabled = bState; }
    inline bool isSimulatingPhysics() { return this->bSimulatePhysics; }
    inline PhysicsMotionType getMotionType() { return motionType; }

    inline void setAsleep()
    {
        bIsSleeping = true;
        linearVelocity = Vector3(0.0f);
        angularVelocity = Vector3(0.0f);
        translationAccumulator = Vector3(0.0f);
    }

    inline void setAwake()
    {
        bIsSleeping = false;
        sleepAccumulator = 0.0f;
    }

    inline ShapeCollisionType getType() { return form->getType(); }
    inline AABB &getAABB() { return aabb; }

    inline Vector3 &getCenterOfMass() { return position; }

    inline PhysicsForm *getForm() { return form; }

    inline PhysicsWorld *getPhysicsWorld() { return world; }

    inline Quat getRotation() { return rotation; }
    inline Vector3 getPosition() { return position; }

    PhysicsBodyCacheTypeSphere *getCacheSphere(int bodyNum) { return &cache[bodyNum].sphere; }
    PhysicsBodyCacheTypePlain *getCachePlain(int bodyNum) { return &cache[bodyNum].plain; }
    PhysicsBodyCacheTypeOBB *getCacheOBB(int bodyNum) { return &cache[bodyNum].OBB; }
    PhysicsBodyCacheTypeConvex *getCacheConvex(int bodyNum) { return &cache[bodyNum].convex; }
    PhysicsBodyCacheTypeCapsule *getCacheCapsule(int bodyNum) { return &cache[bodyNum].capsule; }
    PhysicsBodyCacheTypeMesh *getCacheMesh(int bodyNum) { return &cache[bodyNum].mesh; }

protected:
    inline void checkLimits(float limit)
    {
        if (glm::length(linearVelocity) > limit)
            linearVelocity = glm::normalize(linearVelocity) * limit;
    }
    inline void updateAABB()
    {
        aabbEntity.setPosition(position);
        aabbEntity.setRotation(rotation);
        aabb = form->getAABB(aabbEntity.getModelMatrix());
    }

    // Cached data used by collision dataction
    PhysicsBodyCache *cache = nullptr;
    int cacheBodies = 0;

    // Pointers
    PhysicsWorld *world = nullptr;
    Entity *entity = nullptr;
    PhysicsForm *form = nullptr;
    void *userData = nullptr;
    CollisionHandler *collisionHandler = nullptr;

    // Channel
    Channel channel = CHANNEL_NONE;
    // Constraints
    std::vector<Constraint *> constraints;

    // Main movement parameters
    Vector3 position = Vector3({0.0f, 0.0f, 0.0f});
    Quat rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
    Vector3 linearVelocity = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 angularVelocity = Vector3(0.0f, 0.0f, 0.0f);
    float linearDamping = 0.2f;
    float angularDamping = 0.25f;

    // If body rests for enough time put it to sleep
    float sleepAccumulator = 0.0f;

    // Physics space force, reseted to zero on process
    Vector3 force = Vector3(0.0f, 0.0f, 0.0f);

    // Physics space torque, reseted to zero on process
    Vector3 torque = Vector3(0.0f, 0.0f, 0.0f);

    // Accumulates translation in space from collisions
    Vector3 translationAccumulator = Vector3(0.0f);

    // Motion type
    PhysicsMotionType motionType = PhysicsMotionType::Static;

    // If velocity or translation is lower than this then entity falls asleep
    float sleepCheck;

    // Flags
    bool bIsSleeping = false;
    bool bIsEnabled = true;
    bool bSimulatePhysics = true;

    // Multithreading protection
    std::mutex lock;

    Entity aabbEntity;
    AABB aabb;
};
