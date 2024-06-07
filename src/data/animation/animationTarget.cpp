// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "animationTarget.h"

AnimationTarget::AnimationTarget(std::string targetName)
{
    this->targetName = targetName;
}

float AnimationTarget::getAnimationTimeLength()
{
    if (!keys.empty())
    {
        return keys.at(keys.size() - 1).timeStamp;
    }
    return 0.0f;
}

void AnimationTarget::addKey(AnimationKeyTranform keyTransform)
{
    keys.push_back(keyTransform);
}

void AnimationTarget::getTransformByTime(float timeStamp, Entity *entity)
{
    int size = keys.size();
    for (int i = 0; i < size; i++)
    {
        if (timeStamp < keys[i].timeStamp)
        {
            if (i == 0)
            {
                entity->setPosition(keys[i].position);
                entity->setRotation(keys[i].rotation);
                entity->setScale(keys[i].scale);
            }
            else
            {
                float time = (timeStamp - keys[i - 1].timeStamp) / (keys[i].timeStamp - keys[i - 1].timeStamp);
                entity->setPosition(keys[i - 1].position * (1.0f - time) + keys[i].position * time);
                entity->setRotation(keys[i - 1].rotation * (1.0f - time) + keys[i].rotation * time);
                entity->setScale(keys[i - 1].scale * (1.0f - time) + keys[i].scale * time);
            }
            break;
        }
    }
}