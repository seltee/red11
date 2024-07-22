// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

enum class AudioSourceState
{
    Stopped = 0,
    Playing
};

struct AudioDevice
{
    std::string name;
    bool bIsDefault;
};
