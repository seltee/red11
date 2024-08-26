// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "collisionManifold.h"
#include "physicsUtils.h"
#include <vector>
#include <thread>
#include <mutex>

class PhysicsBody;

class CollisionCollector
{
public:
    EXPORT inline void addBodyPair(PhysicsBody *a, PhysicsBody *b, CollisionManifold manifold)
    {
        lock.lock();
        pairs.push_back({a, b, manifold});
        lock.unlock();
    }

    EXPORT inline void clear() { pairs.clear(); }
    std::vector<CollisionPair> pairs;

protected:
    std::mutex lock;
};