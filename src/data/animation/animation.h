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
    EXPORT Animation(const std::string name);

    EXPORT AnimationTarget *getAnimationTarget(const std::string &targetName);
    EXPORT AnimationTarget *createAnimationTarget(const std::string &targetName);
    EXPORT AnimationTarget *getTargetByName(const std::string &targetName);

    EXPORT bool getAnimationTransformation(const std::string &name, float timeStamp, Entity *entity);

    EXPORT void recalcAnimationLength();

    inline float getTimeLength() { return timeLength; }

    inline std::string getName() { return name; }

protected:
    std::string name;
    std::vector<AnimationTarget *> targets;
    float timeLength = 0.0f;
};