// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXAttribute.h"

FBXAttribute::FBXAttribute(FBXNode *node)
{
    id = *static_cast<unsigned long *>(node->bindedData.at(0).data);
    std::string type = std::string(reinterpret_cast<const char *>(node->bindedData.at(2).data));

    if (type == std::string("LimbNode"))
        bIsLimb = true;
}

FBXAttribute::~FBXAttribute()
{
}

void FBXAttribute::print()
{
    printf("NodeAttribute %llu\n", id);
    if (bIsLimb)
        printf("- Marked as limb\n");
}