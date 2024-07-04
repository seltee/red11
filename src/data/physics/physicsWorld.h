// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/entity.h"
#include "utils/jobQueue.h"
#include "collisionCollector.h"
#include "collisionDispatcher.h"
#include "collisionSolver.h"
#include "physicsBody.h"
#include "physicsForm.h"
#include "physicsUtils.h"
#include <string>

class PhysicsWorld
{
public:
    EXPORT PhysicsWorld();
    EXPORT void setup(Vector3 gravity = Vector3(0.0f, -96.0f, 0.0f), float simScale = 0.1f, float subStep = 0.008f);

    EXPORT void process(float delta);
    EXPORT PhysicsForm *createPhysicsForm(float friction, float restitution, float linearDamping = 0.15f, float angularDamping = 0.05f, float gravityFactor = 1.0f);
    EXPORT PhysicsBody *createPhysicsBody(PhysicsMotionType motionType, PhysicsForm *form, Entity *entity, Vector3 initialPosition, Quat initialRotation);

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

    // Bodies
    std::vector<PhysicsBody *> bodies;

    // Collision information
    CollisionCollector collisionCollector;
    CollisionDispatcher collisionDispatcher;

    // Retrieved from R11
    JobQueue *jobQueue = nullptr;

    // Gravity that affects objects
    Vector3 gravity = Vector3(0.0f, -20.0f, 0.0f);

    // Difference between the sizes of simulation and objects in game to reduce the effect of floating poing inaccuracy
    float simScale = 0.1f;

    // Fixed step that simulation is using
    float subStep = 0.008f;

    // Maximum jobs to add to queue
    int maxJobs = 1;

    // holds delta left from previous frame that is less than subStep
    float deltaAccumulator = 0.0f;

    // Collided pairs
    std::vector<BodyPair> pairs;
};