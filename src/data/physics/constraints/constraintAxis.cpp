// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "constraintAxis.h"

ConstraintAxis::ConstraintAxis(bool bXMovement, bool bYMovement, bool bZMovement, bool bXRotation, bool bYRotation, bool bZRotation)
{
    xMovement = bXMovement ? 1.0f : 0.0f;
    yMovement = bYMovement ? 1.0f : 0.0f;
    zMovement = bZMovement ? 1.0f : 0.0f;
    xRotation = bXRotation ? 1.0f : 0.0f;
    yRotation = bYRotation ? 1.0f : 0.0f;
    zRotation = bZRotation ? 1.0f : 0.0f;
}

ConstraintAxis::ConstraintAxis(const Vector3 &movement, const Vector3 &rotation)
{
    xMovement = movement.x;
    yMovement = movement.y;
    zMovement = movement.z;
    xRotation = rotation.x;
    yRotation = rotation.y;
    zRotation = rotation.z;
}

void ConstraintAxis::processTranslation(Vector3 *translation)
{
    translation->x *= xMovement;
    translation->y *= yMovement;
    translation->z *= zMovement;
}

void ConstraintAxis::processMotion(Vector3 *linearVelocity, Vector3 *angularVelocity)
{
    linearVelocity->x *= xMovement;
    linearVelocity->y *= yMovement;
    linearVelocity->z *= zMovement;
    angularVelocity->x *= xRotation;
    angularVelocity->y *= yRotation;
    angularVelocity->z *= zRotation;
}
