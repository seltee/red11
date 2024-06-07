// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "data/meshObject.h"
#include "utils/primitives.h"
#include <vector>

Mesh *combineMeshList(std::vector<MeshObject *> &list);