// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "boneTransform.h"

BoneTransform::BoneTransform()
{
}

BoneTransform::BoneTransform(Matrix4 *model, std::string *name)
{
    this->model = model;
    this->name = name;
}
