// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "animationTrack.h"

AnimationTrack::AnimationTrack(Animation *animation)
{
    this->animation = animation;
}

void AnimationTrack::play(float speed, float fromMoment, float weight, float weightSwitchSpeed)
{
    this->speed = speed;
    this->bIsPlaying = true;
    this->bIsLooping = false;
    this->playTime = fromMoment;
    this->weight = weight;
    this->targetWeight = weight;
    this->weightSwitchSpeed = weightSwitchSpeed;
}

void AnimationTrack::loop(float speed, float fromMoment, float weight, float weightSwitchSpeed)
{
    this->speed = speed;
    this->bIsPlaying = true;
    this->bIsLooping = true;
    this->playTime = fromMoment;
    this->weight = weight;
    this->targetWeight = weight;
    this->weightSwitchSpeed = weightSwitchSpeed;
}

void AnimationTrack::stop()
{
    this->bIsPlaying = false;
    this->bIsLooping = false;
}

void AnimationTrack::setWeight(float weight)
{
    this->targetWeight = fmaxf(weight, 0.0f);
}

void AnimationTrack::process(float delta)
{
    if (this->bIsPlaying)
    {
        this->playTime += delta * speed;
        while (this->playTime > animation->getTimeLength())
            if (this->bIsLooping)
                this->playTime -= animation->getTimeLength();
            else
                stop();
    }
    if (weight < targetWeight)
    {
        weight += delta * weightSwitchSpeed;
        if (weight > targetWeight)
            weight = targetWeight;
    }
    if (weight > targetWeight)
    {
        weight -= delta * weightSwitchSpeed;
        if (weight < targetWeight)
            weight = targetWeight;
    }
}

float AnimationTrack::getNodeWeight(std::string *name)
{
    auto animator = animation->getTargetByName(*name);
    if (animator)
        return weight;
    return 0.0f;
}

void AnimationTrack::addTransformation(float totalWeight, std::string *nodeName, Vector3 *position, Quat *rotation, Vector3 *scale)
{
    auto animator = animation->getTargetByName(*nodeName);
    if (animator && totalWeight > 0.0f && weight > 0.0f)
    {
        float dWeight = weight / totalWeight;

        Entity entity;
        animator->getTransformByTime(playTime, &entity);
        *position += entity.getPosition() * dWeight;
        *rotation = glm::slerp(*rotation, entity.getRotation(), dWeight);
        *scale += entity.getScale() * dWeight;
    }
}