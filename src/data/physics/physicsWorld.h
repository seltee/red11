// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "utils/segment.h"
#include "data/entity.h"
#include "utils/jobQueue.h"
#include "collisionCollector.h"
#include "collisionDispatcher.h"
#include "collisionSolver.h"
#include "physicsBody.h"
#include "physicsForm.h"
#include "physicsUtils.h"
#include "collisionHandler.h"
#include "constraints/constraint.h"
#include "constraints/constraintAxis.h"
#include "channels.h"
#include <string>
#include <list>

#define DEFAULT_GRAVITY Vector3(0.0f, -96.0f, 0.0f)
#define DEFAULT_SIM_SCALE 1.0f
#define DEFAULT_SUB_STEP 0.006f

class PhysicsWorld
{
public:
    EXPORT PhysicsWorld();
    EXPORT void setup(Vector3 gravity = DEFAULT_GRAVITY, float simScale = DEFAULT_SIM_SCALE, float subStep = DEFAULT_SUB_STEP);

    EXPORT void process(float delta);
    EXPORT PhysicsForm *createPhysicsForm(float friction, float restitution, float linearDamping = 0.15f, float angularDamping = 0.05f, float gravityFactor = 1.0f);
    EXPORT PhysicsBody *createPhysicsBody(PhysicsMotionType motionType, PhysicsForm *form, Entity *entity, Vector3 initialPosition, Quat initialRotation);

    EXPORT void removeBody(PhysicsBody *removeBody);

    EXPORT std::vector<PhysicsBodyPoint> castRayCollision(const Segment &ray, Channel channel);
    EXPORT std::vector<PhysicsBodyPoint> castSphereCollision(const Vector3 &p, float radius, Channel channel);
    EXPORT std::vector<PhysicsBodyPoint> castPointCollision(const Vector3 &p, Channel channel);

    template <class T, typename std::enable_if<std::is_base_of<CollisionHandler, T>::value>::type * = nullptr>
    EXPORT T *createCollisionHandler()
    {
        auto newCollisionHandler = new T();
        prepareNewCollisionHandler(newCollisionHandler);
        return newCollisionHandler;
    }

    inline CollisionCollector *getCollisionCollector() { return &collisionCollector; }
    inline CollisionDispatcher *getCollisionDispatcher() { return &collisionDispatcher; }

    inline float getSimScale() { return simScale; }
    inline float getSubStep() { return subStep; }

    inline Vector3 getGravity() { return gravity; }
    inline void setGravity(Vector3 gravity) { this->gravity = gravity; }

protected:
    // prepare bodies like copy new transformations that came from components
    void prepareBodies();

    // apply resulting transformation onto associated entites
    void finishBodies();

    // add gravity, constraint forces, etc.
    void applyForces();

    // find collided pairs
    void findCollisionPairs();

    // check precise collisions and make collision data
    void findCollisions();

    // apply forces to remove objects from being collided
    void solveCollisions();

    // apply accumulated velocities
    void applyStep();

    // collision events
    void triggerCollisionEvents(CollisionCollector *collisionCollector);

    // remove collisions that ended this cycle
    void updateCollisionTimers(float delta);

    // remove collisions that ended this cycle
    void removeNotPersistedCollisions();

    EXPORT void prepareNewCollisionHandler(CollisionHandler *collisionHandler);

    // Bodies
    std::vector<PhysicsBody *> bodies;

    // Collision information
    CollisionCollector collisionCollector;
    CollisionDispatcher collisionDispatcher;

    // Retrieved from R11
    JobQueue *jobQueue = nullptr;

    // Gravity that affects objects
    Vector3 gravity = DEFAULT_GRAVITY;

    // Difference between the sizes of simulation and objects in game to reduce the effect of floating poing inaccuracy
    float simScale = DEFAULT_SIM_SCALE;

    // Fixed step that simulation is using
    float subStep = DEFAULT_SUB_STEP;

    // Maximum jobs to add to queue
    int maxJobs = 1;

    // holds delta left from previous frame that is less than subStep
    float deltaAccumulator = 0.0f;

    // Collided pairs
    std::vector<BodyPair> pairs;

    // For ray picking
    std::vector<PhysicsBodyPoint> points;

    // All the collision handlers
    std::vector<CollisionHandler *> collisionHanlers;
};