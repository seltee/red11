// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "boneTransform.h"

BoneTransform::BoneTransform()
{
}

BoneTransform::BoneTransform(Matrix4 *model, Deform *deform)
{
    this->model = model;
    this->deform = deform;
}
