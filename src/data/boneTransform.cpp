// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "boneTransform.h"

BoneTransform::BoneTransform()
{
}

BoneTransform::BoneTransform(const Matrix4 *model, const Deform *deform)
{
    this->model = model;
    this->deform = deform;
}
