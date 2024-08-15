// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "entity.h"
#include "utils/glm/gtc/matrix_transform.hpp"

const Matrix4 &Entity::getModelMatrix()
{
    if (bIsTransformationDirty)
    {
        bIsTransformationDirty = false;
        tIteration++;

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