// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"

class Constraint
{
public:
    EXPORT virtual void processTranslation(Vector3 *translation);
    EXPORT virtual void processMotion(Vector3 *linearVelocity, Vector3 *angularVelocity);
};