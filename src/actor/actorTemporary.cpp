#include "actorTemporary.h"

ActorTemporary::ActorTemporary(const std::string &name) : Actor(name)
{
}

void ActorTemporary::onSubProcess(float delta)
{
    timeToExist -= delta;
    if (timeToExist < 0.0f)
    {
        destroy();
    }
}