// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include <string>

class BoneTransform
{
public:
    BoneTransform();
    BoneTransform(Matrix4 *model, std::string *name);

    Matrix4 *model = nullptr;
    std::string *name = nullptr;
};