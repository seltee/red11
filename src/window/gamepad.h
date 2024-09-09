// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"

#define MAX_GAMEPAD_BUTTONS 128
#define MAX_GAMEPAD_AXISES 128

class Gamepad
{
public:
    EXPORT Gamepad();

    // returnes true if state changed
    EXPORT bool checkButtonStateChanged(int buttonNumber, bool state);
    EXPORT bool checkAxisStateChanged(int axisNumber, float state);

    virtual bool is(Gamepad *gamepad) = 0;

    inline unsigned int getVendorId() { return unVendorId; }
    inline unsigned int getProductId() { return unProductId; }
    inline const std::string &getName() { return name; }
    inline const std::string &getDisplayName() { return displayName; }
    inline const char *getUniqueCode() { return uniqueCode; }

    EXPORT int getDefaultCodeA();
    EXPORT int getDefaultCodeB();
    EXPORT int getDefaultCodeX();
    EXPORT int getDefaultCodeY();
    EXPORT int getDefaultCodeCopy();
    EXPORT int getDefaultCodeMenu();
    EXPORT int getDefaultCodeShoulderRT();
    EXPORT int getDefaultCodeShoulderLT();
    EXPORT int getDefaultCodeShoulderRD();
    EXPORT int getDefaultCodeShoulderLD();
    EXPORT int getDefaultCodeDPadUp();
    EXPORT int getDefaultCodeDPadRight();
    EXPORT int getDefaultCodeDPadDown();
    EXPORT int getDefaultCodeDPadLeft();

    EXPORT int getAxisCodeLeftStickHorizontal();
    EXPORT int getAxisCodeLeftStickVertical();
    EXPORT int getAxisCodeRightStickHorizontal();
    EXPORT int getAxisCodeRightStickVertical();

    // 48, 49 - x y left
    // 51, 52 - x y right

    inline float getDeadZone() { return fDeadZone; }
    inline void setDeadZone(float fDeadZone) { this->fDeadZone = fDeadZone; }

protected:
    bool buttonStates[MAX_GAMEPAD_BUTTONS];
    float axisStates[MAX_GAMEPAD_AXISES];

    unsigned int unVendorId = 0;
    unsigned int unProductId = 0;
    std::string name = std::string("Gamepad");
    std::string displayName = std::string("Generic Gamepad");
    char uniqueCode[16];

    float fDeadZone = 0.12f;
};