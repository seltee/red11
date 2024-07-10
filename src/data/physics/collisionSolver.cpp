// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "collisionSolver.h"

CollisionSolver::CollisionSolver(float simScale)
{
    this->simScale = simScale;
    minRenormalVelocity = simScale * -0.02f;
}

void CollisionSolver::solve(PhysicsBody *a, PhysicsBody *b, CollisionManifold &manifold, float delta)
{
    Vector3 normal = manifold.normal[0];
    float depth = manifold.depth[0];
    if (depth <= 0.0f)
        return;

    PhysicsForm *formA = a->getForm();
    PhysicsForm *formB = b->getForm();

    float totalInverseMass = formA->getInvertedMass() + formB->getInvertedMass();
    Vector3 translateA = normal * depth * (formA->getInvertedMass() / totalInverseMass) * 0.25f;
    Vector3 translateB = normal * depth * (formB->getInvertedMass() / totalInverseMass) * 0.25f;

    if (a->getMotionType() != PhysicsMotionType::Static)
        a->translate((b->getMotionType() != PhysicsMotionType::Static) ? -translateA : -(translateA + translateB));
    if (b->getMotionType() != PhysicsMotionType::Static)
        b->translate((a->getMotionType() != PhysicsMotionType::Static) ? translateB : translateA + translateB);

    Vector3 pointA = manifold.pointsOnA[0];
    Vector3 pointB = manifold.pointsOnB[0];
    Vector3 localPointA = pointA - a->getCenterOfMass();
    Vector3 localPointB = pointB - b->getCenterOfMass();

    // Calculate velocity of collision points
    Vector3 pointVelocityBodyA = a->getPointVelocity(localPointA);
    Vector3 pointVelocityBodyB = b->getPointVelocity(localPointB);
    Vector3 relativeVelocity = pointVelocityBodyB - pointVelocityBodyA;

    float friction = sqrtf(formA->getFriction() * formB->getFriction());

    float normalVelocity = glm::dot(relativeVelocity, normal);
    if (normalVelocity > 0.0f)
        return;

    // Calculate velocity bias due to restitution
    float combinedRestitution = fmaxf(formA->getRestitution(), formB->getRestitution());
    float normalVelocityBias = 0.0f;
    if (combinedRestitution > 0.0f && normalVelocity < minRenormalVelocity)
        normalVelocityBias = combinedRestitution * normalVelocity;

    float lambda = solveAxis(
        a,
        b,
        localPointA,
        localPointB,
        pointVelocityBodyA,
        pointVelocityBodyB,
        a->getAngularVelocity(),
        b->getAngularVelocity(),
        normal,
        -3.4f,
        3.4f,
        normalVelocityBias);

    if (friction > 0.0f)
    {
        // Apply friction
        Vector3 tangent1 = getNormalizedPerpendicular(normal);
        Vector3 tangent2 = glm::cross(normal, tangent1);

        float maxLambda = friction * lambda;

        solveAxis(
            a,
            b,
            localPointA,
            localPointB,
            a->getLinearVelocity(),
            b->getLinearVelocity(),
            a->getAngularVelocity(),
            b->getAngularVelocity(),
            tangent1,
            -maxLambda,
            maxLambda,
            0.0f);

        solveAxis(
            a,
            b,
            localPointA,
            localPointB,
            a->getLinearVelocity(),
            b->getLinearVelocity(),
            a->getAngularVelocity(),
            b->getAngularVelocity(),
            tangent2,
            -maxLambda,
            maxLambda,
            0.0f);
    }
}

float CollisionSolver::solveAxis(
    PhysicsBody *a,
    PhysicsBody *b,
    Vector3 pointA,
    Vector3 pointB,
    Vector3 velocityBodyA,
    Vector3 velocityBodyB,
    Vector3 angularVelocityBodyA,
    Vector3 angularVelocityBodyB,
    Vector3 axis,
    float fMin,
    float fMax,
    float bias)
{
    PhysicsForm *formA = a->getForm();
    PhysicsForm *formB = b->getForm();

    Vector3 r1Axis = Vector3(0.0f);
    if (a->getMotionType() != PhysicsMotionType::Static)
        r1Axis = glm::cross(pointA, axis);

    Vector3 r2Axis = Vector3(0.0f);
    if (b->getMotionType() != PhysicsMotionType::Static)
        r2Axis = glm::cross(pointB, axis);

    // Inverse effective mass: K = J M^-1 J^T
    float invEffectiveMass = 0.0f;

    Vector3 mInvI1_R1Axis(0.0f);
    if (a->getMotionType() == PhysicsMotionType::Dynamic)
    {
        mInvI1_R1Axis = formA->getInvertedInertia() * r1Axis;
        invEffectiveMass = formA->getInvertedMass() + glm::dot(mInvI1_R1Axis, r1Axis);
    }
    else
        invEffectiveMass = 0.0f;

    Vector3 mInvI2_R2Axis(0.0f);
    if (b->getMotionType() == PhysicsMotionType::Dynamic)
    {
        mInvI2_R2Axis = formB->getInvertedInertia() * r2Axis;
        invEffectiveMass += formB->getInvertedMass() + glm::dot(mInvI2_R2Axis, r2Axis);
    }

    float jv = 0.0f;
    if (a->getMotionType() != PhysicsMotionType::Static && b->getMotionType() != PhysicsMotionType::Static)
        jv = glm::dot(axis, velocityBodyA - velocityBodyB);
    else if (a->getMotionType() != PhysicsMotionType::Static)
        jv = glm::dot(axis, velocityBodyA);
    else if (b->getMotionType() != PhysicsMotionType::Static)
        jv = glm::dot(axis, -velocityBodyB);

    // Calculate jacobian multiplied by angular velocity
    if (a->getMotionType() != PhysicsMotionType::Static)
    {
        jv += glm::dot(r1Axis, angularVelocityBodyA);
    }
    if (b->getMotionType() != PhysicsMotionType::Static)
    {
        jv -= glm::dot(r2Axis, angularVelocityBodyB);
    }

    float mEffectiveMass = 1.0f / invEffectiveMass;
    float lambda = fminf(fmaxf(fMin, mEffectiveMass * (jv - bias)), fMax);

    if (lambda != 0.0f)
    {
        a->addLinearVelocity(-(lambda * formA->getInvertedMass()) * axis);
        a->addAngularVelocity(-lambda * mInvI1_R1Axis);

        b->addLinearVelocity((lambda * formB->getInvertedMass()) * axis);
        b->addAngularVelocity(lambda * mInvI2_R2Axis);
    }

    return lambda;
}
