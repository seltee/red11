// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "constraint.h"

class ConstraintAxis : public Constraint
{
public:
    EXPORT ConstraintAxis(bool bXMovement, bool bYMovement, bool bZMovement, bool bXRotation, bool bYRotation, bool bZRotation);

    EXPORT void processTranslation(Vector3 *translation) override;
    EXPORT void processMotion(Vector3 *linearVelocity, Vector3 *angularVelocity) override;

protected:
    float xMovement, yMovement, zMovement;
    float xRotation, yRotation, zRotation;
};