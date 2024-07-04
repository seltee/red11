// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "physicsWorld.h"
#include "red11.h"
#include <mutex>
#include <chrono>

EXPORT PhysicsWorld::PhysicsWorld()
{
    jobQueue = Red11::getJobQueue();
    maxJobs = min(jobQueue->getMaxJobs() * 2, 32);
}

void PhysicsWorld::setup(Vector3 gravity, float simScale, float subStep)
{
    this->gravity = gravity;
    this->simScale = simScale;
    this->subStep = subStep;
}

void PhysicsWorld::process(float delta)
{
    if (bodies.size() == 0)
        return;

    deltaAccumulator += delta;
    if (deltaAccumulator < subStep)
        return;

    prepareBodies();
    while (deltaAccumulator > subStep)
    {
        deltaAccumulator -= subStep;

        applyForces();
        findCollisionPairs();
        findCollisions();
        solveCollisions();
        applyStep();
        // triggerCollisionEvents(&collisionCollector);
        // removeNotPersistedCollisions();
    }
    finishBodies();
}

PhysicsForm *PhysicsWorld::createPhysicsForm(float friction, float restitution, float linearDamping, float angularDamping, float gravityFactor)
{
    PhysicsForm *form = new PhysicsForm(simScale, friction, restitution, linearDamping, angularDamping, gravityFactor);
    return form;
}

PhysicsBody *PhysicsWorld::createPhysicsBody(PhysicsMotionType motionType, PhysicsForm *form, Entity *entity, Vector3 initialPosition, Quat initialRotation)
{
    PhysicsBody *newBody = new PhysicsBody(motionType, form, this, entity, initialPosition * simScale, initialRotation);
    bodies.push_back(newBody);
    return newBody;
}

void PhysicsWorld::prepareBodies()
{
    if (maxJobs > bodies.size())
    {
        _prepareBody(bodies.begin(), bodies.end());
    }
    else
    {
        int bodiesPerThread = bodies.size() / maxJobs;

        std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
        for (int i = 0; i < maxJobs; i++)
        {
            auto end = (i == maxJobs - 1) ? bodies.end() : currentBody + bodiesPerThread;

            jobQueue->queueJob([currentBody, end]
                               { _prepareBody(currentBody, end); });

            currentBody += bodiesPerThread;
        }
        while (jobQueue->isBusy())
        {
        };
    }
}

void PhysicsWorld::finishBodies()
{
    if (maxJobs > bodies.size())
    {
        _finishBody(bodies.begin(), bodies.end());
    }
    else
    {
        int bodiesPerThread = bodies.size() / maxJobs;
        std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
        for (int i = 0; i < maxJobs; i++)
        {
            auto end = (i == maxJobs - 1) ? bodies.end() : currentBody + bodiesPerThread;

            jobQueue->queueJob([currentBody, end]
                               { _finishBody(currentBody, end); });

            currentBody += bodiesPerThread;
        }
        while (jobQueue->isBusy())
        {
        };
    }
}

void PhysicsWorld::applyForces()
{
    float subStep = this->subStep;
    Vector3 localGravity = gravity * simScale;
    if (maxJobs > bodies.size())
    {
        _processBody(bodies.begin(), bodies.end(), subStep, localGravity);
    }
    else
    {
        int bodiesPerThread = bodies.size() / maxJobs;
        std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();
        for (int i = 0; i < maxJobs; i++)
        {
            auto end = (i == maxJobs - 1) ? bodies.end() : currentBody + bodiesPerThread;
            jobQueue->queueJob([currentBody, end, subStep, localGravity]
                               { _processBody(currentBody, end, subStep, localGravity); });

            currentBody += bodiesPerThread;
        }
        while (jobQueue->isBusy())
        {
        };
    }
}

void PhysicsWorld::findCollisionPairs()
{
    pairs.clear();
    if (maxJobs > bodies.size())
    {
        _collectPairs(bodies.begin(), bodies.end(), &bodies, &pairs);
    }
    else
    {
        // find possible collision pairs
        int bodiesPerThread = bodies.size() / maxJobs;
        std::vector<PhysicsBody *> *pBodies = &bodies;
        std::vector<BodyPair> *pPairs = &pairs;
        std::vector<PhysicsBody *>::iterator currentBody = pBodies->begin();
        for (int i = 0; i < maxJobs; i++)
        {
            auto end = (i == maxJobs - 1) ? pBodies->end() : currentBody + bodiesPerThread;

            jobQueue->queueJob([currentBody, end, pBodies, pPairs]
                               { _collectPairs(currentBody, end, pBodies, pPairs); });

            currentBody += bodiesPerThread;
        }
        while (jobQueue->isBusy())
        {
        };
    }
}

void PhysicsWorld::findCollisions()
{
    // find exact collisions
    collisionCollector.clear();
    if (maxJobs > pairs.size())
    {
        _collide(pairs.begin(), pairs.end(), &collisionDispatcher, &collisionCollector);
    }
    else
    {
        int pairsPerThread = pairs.size() / maxJobs;
        std::vector<BodyPair>::iterator currentPair = pairs.begin();
        auto pCollisionDispatcher = &collisionDispatcher;
        auto pCollisionCollector = &collisionCollector;

        for (int i = 0; i < maxJobs; i++)
        {
            auto end = (i == maxJobs - 1) ? pairs.end() : currentPair + pairsPerThread;

            jobQueue->queueJob([currentPair, end, pCollisionDispatcher, pCollisionCollector]
                           { _collide(currentPair, end, pCollisionDispatcher, pCollisionCollector); });

            currentPair += pairsPerThread;
        }
        while (jobQueue->isBusy())
        {
        };
    }
}

void PhysicsWorld::solveCollisions()
{
    if (maxJobs > collisionCollector.pairs.size())
    {
        _solve(collisionCollector.pairs.begin(), collisionCollector.pairs.end(), simScale, subStep);
    }
    else
    {
        if (!collisionCollector.pairs.empty())
        {
            int pairsPerThread = collisionCollector.pairs.size() / maxJobs;
            std::vector<CollisionPair>::iterator currentCollisionPair = collisionCollector.pairs.begin();

            float simScale = this->simScale;
            float subStep = this->subStep;
            for (int i = 0; i < maxJobs; i++)
            {
                auto end = (i == maxJobs - 1) ? collisionCollector.pairs.end() : currentCollisionPair + pairsPerThread;

                jobQueue->queueJob([currentCollisionPair, end, simScale, subStep]
                               { _solve(currentCollisionPair, end, simScale, subStep); });

                currentCollisionPair += pairsPerThread;
            }
            while (jobQueue->isBusy())
            {
            };
        }
    }
}

void PhysicsWorld::applyStep()
{
    float subStep = this->subStep;
    if (maxJobs > bodies.size())
    {
        _applyStepBody(bodies.begin(), bodies.end(), subStep);
    }
    else
    {
        int bodiesPerThread = bodies.size() / maxJobs;
        std::vector<PhysicsBody *>::iterator currentBody = bodies.begin();

        for (int i = 0; i < maxJobs; i++)
        {
            auto end = (i == maxJobs - 1) ? bodies.end() : currentBody + bodiesPerThread;

            jobQueue->queueJob([currentBody, end, subStep]
                               { _applyStepBody(currentBody, end, subStep); });

            currentBody += bodiesPerThread;
        }
        while (jobQueue->isBusy())
        {
        };
    }
}