// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "data/deform.h"
#include <string>

class BoneTransform
{
public:
    BoneTransform();
    BoneTransform(const Matrix4 *model, const Deform *deform);

    const Matrix4 *model = nullptr;
    const Deform *deform = nullptr;
};