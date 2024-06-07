// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "animationTrack.h"

AnimationTrack::AnimationTrack(Animation *animation)
{
    this->animation = animation;
}

void AnimationTrack::play(float speed, float fromMoment, float weight)
{
    this->speed = speed;
    this->bIsPlaying = true;
    this->bIsLooping = false;
    this->playTime = fromMoment;
    this->weight = weight;
}

void AnimationTrack::loop(float speed, float fromMoment, float weight)
{
    this->speed = speed;
    this->bIsPlaying = true;
    this->bIsLooping = true;
    this->playTime = fromMoment;
    this->weight = weight;
}

void AnimationTrack::stop()
{
    this->bIsPlaying = false;
    this->bIsLooping = false;
}

void AnimationTrack::setWeight(float weight)
{
    this->weight = weight;
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
}

void AnimationTrack::addTransformation(float totalWeight, std::string *nodeName, Vector3 *position, Quat *rotation, Vector3 *scale)
{
    float dWeight = weight / totalWeight;

    auto animator = animation->getTargetByName(*nodeName);
    if (animator)
    {
        Entity entity;
        animator->getTransformByTime(playTime, &entity);
        *position += entity.getPosition() * dWeight;
        *rotation += entity.getRotation() * dWeight;
        *scale += entity.getScale() * dWeight;
    }
}