// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

class Destroyable
{
public:
    EXPORT void destroy();
    EXPORT bool isDestroyed();
    EXPORT virtual void onDestroy();

protected:
    bool bMarkedToDestroy = false;
};
