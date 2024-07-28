#pragma once
#include "utils/utils.h"
#include "uiUtils.h"

class UINode;

class UIEventController
{
public:
    virtual void processEvent(UIEvent ev, UINode *node) = 0;

protected:
};
