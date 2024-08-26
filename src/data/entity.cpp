// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "entity.h"
#include "utils/glm/gtc/matrix_transform.hpp"

const Matrix4 &Entity::getModelMatrix()
{
    if (bIsTransformationDirty || bRotationIsDirty)
    {
        bIsTransformationDirty = false;
        bRotationIsDirty = false;
        tIteration++;

        if (bRotationIsDirty)
            rotation = glm::normalize(rotation);

        mModelLocal = glm::translate(Matrix4(1.0f), position);
        mModelLocal *= glm::toMat4(rotation);
        if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
        {
            mModelLocal *= glm::scale(Matrix4(1.0f), scale);
        }

        mModelWithParent = transformationParent ? transformationParent->getModelMatrix() * mModelLocal : mModelLocal;
        tLastParentIteration = transformationParent ? transformationParent->getTIteration() : 0;
    }
    else
    {
        if (transformationParent && (transformationParent->getTIteration() != tIteration || transformationParent->isTransformationDirty()))
        {
            tIteration++;
            mModelWithParent = transformationParent->getModelMatrix() * mModelLocal;
            tLastParentIteration = transformationParent->getTIteration();
        }
    }

    return mModelWithParent;
}

void Entity::lookAt(const Vector3 &point)
{
    bIsTransformationDirty = true;

    Vector3 absolutePosition = Vector3(getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    Vector3 normal = glm::normalize(point - absolutePosition);

    float y = atan2f(normal.z, normal.x);
    float len = sqrtf(normal.x * normal.x + normal.z * normal.z);
    float x = atan2(len, normal.y);

    setRotation(Vector3(CONST_PI / 2 - x, -y - CONST_PI / 2.0f, 0.0f));
}
