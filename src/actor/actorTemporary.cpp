// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "actorTemporary.h"

ActorTemporary::ActorTemporary(const std::string &name) : Actor(name)
{
}

void ActorTemporary::onProcess(float delta)
{
    timeToExist -= delta;
    if (timeToExist < 0.0f)
    {
        destroy();
    }
}