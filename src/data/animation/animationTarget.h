// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/math.h"
#include "data/entity.h"
#include "utils/utils.h"
#include <string>

// Time in timestamp is 1.0 = 1000 ms or 1 second
// Due to float precision sweat spot

struct AnimationKeyTranform
{
    float timeStamp;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

class AnimationTarget
{
public:
    EXPORT AnimationTarget(const std::string targetName);

    EXPORT float getAnimationTimeLength();

    // interpolates between 2 frames
    EXPORT void getTransformByTime(float timeStamp, Entity *entity);
    // picks static
    EXPORT void getTransformByTimeFixedFrame(float timeStamp, Entity *entity);

    inline bool isName(std::string targetName) { return this->targetName == targetName; }
    inline std::string getTargetName() { return targetName; }

    void addKey(AnimationKeyTranform keyTransform);

protected:
    std::string targetName;
    std::vector<AnimationKeyTranform> keys;
};