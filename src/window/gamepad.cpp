// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "gamepad.h"
#include <string>

#define GAMEPAD_A 1
#define GAMEPAD_B 2
#define GAMEPAD_X 3
#define GAMEPAD_Y 4
#define GAMEPAD_SHOULDER_RT 5
#define GAMEPAD_SHOULDER_LT 6
#define GAMEPAD_COPY 7
#define GAMEPAD_MENU 8
#define GAMEPAD_SHOULDER_RD 9
#define GAMEPAD_SHOULDER_LD 10
#define GAMEPAD_DPAD_UP 124
#define GAMEPAD_DPAD_RIGHT 125
#define GAMEPAD_DPAD_DOWN 126
#define GAMEPAD_DPAD_LEFT 127

#define GAMEPAD_AXIS_STICK_LEFT_H 48
#define GAMEPAD_AXIS_STICK_LEFT_V 49
#define GAMEPAD_AXIS_STICK_RIGHT_H 51
#define GAMEPAD_AXIS_STICK_RIGHT_V 52

Gamepad::Gamepad()
{
    memset(buttonStates, 0, sizeof(bool) * MAX_GAMEPAD_BUTTONS);
    memset(axisStates, 0, sizeof(float) * MAX_GAMEPAD_AXISES);
}

bool Gamepad::checkButtonStateChanged(int buttonNumber, bool state)
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

bool Gamepad::checkAxisStateChanged(int axisNumber, float state)
{
    if (axisNumber >= 0 && axisNumber < MAX_GAMEPAD_AXISES)
    {
        if (axisStates[axisNumber] != state)
        {
            axisStates[axisNumber] = state;
            return true;
        }
    }
    return false;
}

int Gamepad::getDefaultCodeA()
{
    return GAMEPAD_A;
}

int Gamepad::getDefaultCodeB()
{
    return GAMEPAD_B;
}

int Gamepad::getDefaultCodeX()
{
    return GAMEPAD_X;
}

int Gamepad::getDefaultCodeY()
{
    return GAMEPAD_Y;
}

int Gamepad::getDefaultCodeCopy()
{
    return GAMEPAD_COPY;
}

int Gamepad::getDefaultCodeMenu()
{
    return GAMEPAD_MENU;
}

int Gamepad::getDefaultCodeShoulderRT()
{
    return GAMEPAD_SHOULDER_RT;
}

int Gamepad::getDefaultCodeShoulderLT()
{
    return GAMEPAD_SHOULDER_LT;
}

int Gamepad::getDefaultCodeShoulderRD()
{
    return GAMEPAD_SHOULDER_RD;
}

int Gamepad::getDefaultCodeShoulderLD()
{
    return GAMEPAD_SHOULDER_LD;
}

int Gamepad::getDefaultCodeDPadUp()
{
    return GAMEPAD_DPAD_UP;
}

int Gamepad::getDefaultCodeDPadRight()
{
    return GAMEPAD_DPAD_RIGHT;
}

int Gamepad::getDefaultCodeDPadDown()
{
    return GAMEPAD_DPAD_DOWN;
}

int Gamepad::getDefaultCodeDPadLeft()
{
    return GAMEPAD_DPAD_LEFT;
}

int Gamepad::getAxisCodeLeftStickHorizontal()
{
    return GAMEPAD_AXIS_STICK_LEFT_H;
}

int Gamepad::getAxisCodeLeftStickVertical()
{
    return GAMEPAD_AXIS_STICK_LEFT_V;
}

int Gamepad::getAxisCodeRightStickHorizontal()
{
    return GAMEPAD_AXIS_STICK_RIGHT_H;
}

int Gamepad::getAxisCodeRightStickVertical()
{
    return GAMEPAD_AXIS_STICK_RIGHT_V;
}