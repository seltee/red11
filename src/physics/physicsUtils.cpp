#include "physicsUtils.h"
#include "physicsBody.h"
#include "collisionSolver.h"
#include "collisionCollector.h"
#include "collisionDispatcher.h"

std::mutex lock;

void _prepareBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->prepareForSimulation();
}

void _finishBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->finishSimulation();
}

void _processBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd, float subStep, Vector3 localGravity)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->processStep(subStep, localGravity);
}

void _applyStepBody(std::vector<PhysicsBody *>::iterator bodyStart, std::vector<PhysicsBody *>::iterator bodyEnd, float subStep)
{
    for (auto body = bodyStart; body < bodyEnd; body++)
        (*body)->applyStep(subStep);
}

void _collectPairs(
    std::vector<PhysicsBody *>::iterator bodyStart,
    std::vector<PhysicsBody *>::iterator bodyEnd,
    std::vector<PhysicsBody *> *bodyList,
    std::vector<BodyPair> *list)
{
    for (auto a = bodyStart; a < bodyEnd; a++)
    {
        if (!(*a)->isEnabled())
            continue;
        for (auto b = bodyList->begin(); b != bodyList->end(); b++)
        {
            if (a == b)
                break;

            if (!(*b)->isEnabled())
                continue;

            if (!((*a)->getChannel() & CHANNEL_SIMULATION))
                continue;

            if (!((*b)->getChannel() & CHANNEL_SIMULATION))
                continue;

            if ((*a)->getMotionType() == PhysicsMotionType::Static && (*b)->getMotionType() == PhysicsMotionType::Static)
                continue;

            if ((*a)->isSleeping() && (*b)->isSleeping())
                continue;

            if ((*a)->getAABB().test((*b)->getAABB()))
            {
                lock.lock();
                list->push_back({*a, *b});
                lock.unlock();
            }
        }
    }
}

void _collide(
    std::vector<BodyPair>::iterator pairStart,
    std::vector<BodyPair>::iterator pairEnd,
    CollisionDispatcher *collisionDispatcher,
    CollisionCollector *collisionCollector)
{
    for (auto pair = pairStart; pair < pairEnd; pair++)
    {
        collisionDispatcher->collide(pair->a, pair->b, collisionCollector);
    }
}

void _solve(
    std::vector<CollisionPair>::iterator pairStart,
    std::vector<CollisionPair>::iterator pairEnd,
    float simScale,
    float subStep)
{
    CollisionSolver collisionSolver(simScale);
    for (auto pair = pairStart; pair < pairEnd; pair++)
    {
        collisionSolver.solve(pair->a, pair->b, pair->manifold, subStep);
    }
}

void _ray(
    std::vector<PhysicsBody *>::iterator bodyStart,
    std::vector<PhysicsBody *>::iterator bodyEnd,
    const Segment &rayLocal,
    std::vector<PhysicsBodyPoint> *points,
    Channel channel)
{
    PhysicsBodyPoint newPoints[8];

    for (auto body = bodyStart; body < bodyEnd; body++)
    {
        if (!((*body)->getChannel() & channel))
            continue;

        if (!(*body)->getAABB().test(rayLocal))
            continue;

        int pointsCollected = (*body)->castRay(rayLocal, newPoints);
        if (pointsCollected)
        {
            lock.lock();
            for (int i = 0; i < pointsCollected; i++)
                points->push_back(newPoints[i]);
            lock.unlock();
        }
    }
}
