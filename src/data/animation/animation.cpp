// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "animation.h"

Animation::Animation(std::string name)
{
    this->name = name;
}

AnimationTarget *Animation::getAnimationTarget(std::string &name)
{
    for (auto &target : targets)
    {
        if (target->isName(name))
            return target;
    }
    return nullptr;
}

AnimationTarget *Animation::createAnimationTarget(std::string &targetName)
{
    auto existing = getAnimationTarget(targetName);
    if (!existing)
    {
        auto newAnimationTarget = new AnimationTarget(targetName);
        targets.push_back(newAnimationTarget);
        return newAnimationTarget;
    }
    return existing;
}

AnimationTarget *Animation::getTargetByName(std::string &targetName)
{
    for (auto &target : targets)
    {
        if (target->getTargetName() == targetName)
        {
            return target;
        }
    }
    return nullptr;
}

bool Animation::getAnimationTransformation(std::string &name, float timeStamp, Entity *entity)
{
    for (auto &target : targets)
    {
        if (target->getTargetName() == name)
        {
            target->getTransformByTime(timeStamp, entity);
            return true;
        }
    }
    return false;
}

void Animation::recalcAnimationLength()
{
    timeLength = 0.0f;
    for (auto &target : targets)
    {
        timeLength = fmaxf(target->getAnimationTimeLength(), timeLength);
    }
}