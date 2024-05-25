// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "utils/destroyable.h"

EXPORT void Destroyable::destroy()
{
    if (!bMarkedToDestroy)
    {
        bMarkedToDestroy = true;
        onDestroy();
    }
}

EXPORT bool Destroyable::isDestroyed()
{
    return bMarkedToDestroy;
}

EXPORT void Destroyable::onDestroy()
{
}