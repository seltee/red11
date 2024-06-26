// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "animator.h"

Animator::Animator(Animation *animation)
{
    this->animation = animation;
}

void Animator::process(float delta)
{
    if (this->animation && bIsPlaying)
    {
        float timeLength = this->animation->getTimeLength();
        time += delta * speed;

        if (time > timeLength)
            time -= timeLength;
        if (time < 0.0)
            time += timeLength;
    }
}