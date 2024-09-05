// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "actor.h"
#include "utils/utils.h"

class ActorTemporary : public Actor
{
public:
    EXPORT ActorTemporary(const std::string &name);
    EXPORT void onProcess(float delta) override;

    inline void setTimeToExist(float time) { timeToExist = time; }

protected:
    float timeToExist = 1.0f;
};