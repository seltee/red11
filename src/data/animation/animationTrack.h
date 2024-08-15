// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "animation.h"

class AnimationTrack
{
public:
    EXPORT AnimationTrack(Animation *animation);
    EXPORT void play(float speed = 1.0f, float fromMoment = 0.0f, float weight = 1.0f, float weightSwitchSpeed = 4.0f);
    EXPORT void loop(float speed = 1.0f, float fromMoment = 0.0f, float weight = 1.0f, float weightSwitchSpeed = 4.0f);
    EXPORT void stop();
    EXPORT void setWeight(float weight);
    EXPORT void process(float delta);

    float getNodeWeight(const std::string &name);

    EXPORT void addTransformation(float totalWeight, const std::string &nodeName, Vector3 *position, Quat *rotation, Vector3 *scale);

    inline bool isPlaying() { return bIsPlaying; }
    inline bool isLooping() { return bIsLooping; }
    inline float getWeight() { return weight; }
    inline void setSpeed(float speed) { this->speed = speed; }
    inline float getSpeed() { return speed; }
    inline void setWeightSwitchSpeed(float weightSwitchSpeed) { this->weightSwitchSpeed = weightSwitchSpeed; }
    inline float getWeightSwitchSpeed() { return weightSwitchSpeed; }

protected:
    Animation *animation = nullptr;
    bool bIsPlaying = false;
    bool bIsLooping = false;
    float playTime = 0.0f;
    float weight = 0.0f;
    float targetWeight = 0.0f;
    float speed = 1.0f;
    float weightSwitchSpeed = 0.0f;
};