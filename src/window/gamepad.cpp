// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "gamepad.h"
#include <string>

Gamepad::Gamepad()
{
    memset(buttonStates, 0, sizeof(bool) * MAX_GAMEPAD_BUTTONS);
}

bool Gamepad::checkStateChanged(int buttonNumber, bool state)
{
    if (buttonNumber >= 0 && buttonNumber < MAX_GAMEPAD_BUTTONS)
    {
        if (buttonStates[buttonNumber] != state)
        {
            buttonStates[buttonNumber] = state;
            return true;
        }
    }
    return false;
}
