#pragma once
#include "actor.h"
#include "utils/utils.h"

class ActorTemporary : public Actor
{
public:
    EXPORT ActorTemporary(const std::string &name);
    EXPORT void onSubProcess(float delta) override;

    inline void setTimeToExist(float time) { timeToExist = time; }

protected:
    float timeToExist = 1.0f;
};