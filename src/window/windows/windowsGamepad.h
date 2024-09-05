// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "window/gamepad.h"
#include "utils/utils.h"

#ifdef WINDOWS_ONLY
#include <shellscalingapi.h>
#include <windows.h>

class WindowsGamepad : public Gamepad
{
public:
    EXPORT WindowsGamepad(HANDLE hDevice);
    inline HANDLE getHandle() { return hDevice; }
    EXPORT bool is(Gamepad *gamepad) override final;

protected:
    std::string getName();
    std::string getDisplayName();

    HANDLE hDevice;
};
#endif