// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "settings.h"
#include "utils/utils.h"
#include "window/gamepad.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <XInput.h>
#include <hidsdi.h>
#include <hidusage.h>
#include <hidpi.h>
#include <algorithm>
#include <shellscalingapi.h>

struct GamepadReport
{
    bool buttons[MAX_GAMEPAD_BUTTONS];
};

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
GamepadReport parseHidReport(HANDLE hDevice, PHIDP_PREPARSED_DATA pPreparsedData, char *report, UINT reportLength);
void enableGamepadInput(HWND hWnd);

#endif