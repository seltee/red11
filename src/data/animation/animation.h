// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/entity.h"
#include "animationTarget.h"
#include "utils/utils.h"
#include <string>
#include <vector>

class Animation
{
public:
    EXPORT Animation(std::string name);

    EXPORT AnimationTarget *getAnimationTarget(std::string &targetName);
    EXPORT AnimationTarget *createAnimationTarget(std::string &targetName);
    EXPORT AnimationTarget *getTargetByName(std::string &targetName);

    EXPORT bool getAnimationTransformation(std::string &name, float timeStamp, Entity *entity);

    EXPORT void recalcAnimationLength();

    inline float getTimeLength() { return timeLength; }

    inline std::string getName() { return name; }

protected:
    std::string name;
    std::vector<AnimationTarget *> targets;
    float timeLength = 0.0f;
};