// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/entity.h"
#include "animationTarget.h"
#include <string>
#include <vector>

class Animation
{
public:
    Animation(std::string name);

    AnimationTarget *getAnimationTarget(std::string &targetName);
    AnimationTarget *createAnimationTarget(std::string &targetName);
    AnimationTarget *getTargetByName(std::string &targetName);

    bool getAnimationTransformation(std::string &name, float timeStamp, Entity *entity);

    void recalcAnimationLength();

    inline float getTimeLength() { return timeLength; }

    inline std::string getName() { return name; }

protected:
    std::string name;
    std::vector<AnimationTarget *> targets;
    float timeLength = 0.0f;
};