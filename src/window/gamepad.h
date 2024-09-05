// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#define MAX_GAMEPAD_BUTTONS 128
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

class Gamepad
{
public:
    EXPORT Gamepad();

    // returnes true if state changed
    EXPORT bool checkStateChanged(int buttonNumber, bool state);

    virtual bool is(Gamepad *gamepad) = 0;

    inline unsigned int getVendorId() { return unVendorId; }
    inline unsigned int getProductId() { return unProductId; }
    inline const std::string &getName() { return name; }
    inline const std::string &getDisplayName() { return displayName; }
    inline const char *getUniqueCode() { return uniqueCode; }

    inline int getDefaultCodeA() { return GAMEPAD_A; }
    inline int getDefaultCodeB() { return GAMEPAD_B; }
    inline int getDefaultCodeX() { return GAMEPAD_X; }
    inline int getDefaultCodeY() { return GAMEPAD_Y; }
    inline int getDefaultCodeCopy() { return GAMEPAD_COPY; }
    inline int getDefaultCodeMenu() { return GAMEPAD_MENU; }
    inline int getDefaultCodeShoulderRT() { return GAMEPAD_SHOULDER_RT; }
    inline int getDefaultCodeShoulderLT() { return GAMEPAD_SHOULDER_LT; }
    inline int getDefaultCodeShoulderRD() { return GAMEPAD_SHOULDER_RD; }
    inline int getDefaultCodeShoulderLD() { return GAMEPAD_SHOULDER_LD; }
    inline int getDefaultCodeDPadUp() { return GAMEPAD_DPAD_UP; }
    inline int getDefaultCodeDPadRight() { return GAMEPAD_DPAD_RIGHT; }
    inline int getDefaultCodeDPadDown() { return GAMEPAD_DPAD_DOWN; }
    inline int getDefaultCodeDPadLeft() { return GAMEPAD_DPAD_LEFT; }

protected:
    bool buttonStates[MAX_GAMEPAD_BUTTONS];

    unsigned int unVendorId = 0;
    unsigned int unProductId = 0;
    std::string name = std::string("Gamepad");
    std::string displayName = std::string("Generic Gamepad");
    char uniqueCode[16];
};