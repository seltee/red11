// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "utils/keyboardCodes.h"
#include "window/gamepad.h"
#include <vector>

enum class InputMouseType
{
    LeftButton,
    RightButton,
    MiddleButton,
    Wheel,
    MoveX,
    MoveY,
    PositionX,
    PositionY
};

enum class InputType
{
    Mouse,
    Keyboard,
    GamepadButton,
    GamepadAxis
};

struct InputMouse
{
    InputMouseType type;
    int value;
};

struct InputKeyboard
{
    bool state;
    KeyboardCode keyCode;
};

struct InputGamepadButton
{
    bool state;
    int keyCode;
    Gamepad *gamepad;
};

struct InputGamepadAxis
{
    float value;
    int axisCode;
};

union InputData
{
    InputMouse mouse;
    InputKeyboard keyboard;
    InputGamepadButton gamepadButton;
    InputGamepadAxis gamepadAxis;
};

union InputAbstractValue
{
    InputMouseType mouseInput;
    KeyboardCode keyboardKeyCodeInput;
    int gamepadButtonInput;
    int gamepadAxisInput;
};

struct InputDescriptor
{
    InputType type;
    InputAbstractValue input;
    float modifier;
    void *data;
};

struct InputDescriptorCallbackData
{
    unsigned int index;
    std::vector<InputDescriptor> descriptors;
    void *userData;
    void (*callback)(InputType type, InputData *data, float value, void *userData);
};

struct InputDetectorCallbackData
{
    void (*callback)(InputType type);
};

typedef void (*InputAnyCallback)(void *userData);
struct InputAny
{
    InputAnyCallback callback;
    void *userData;
    int index = 0;
};

class InputDescriptorList
{
public:
    std::vector<InputDescriptor> descriptors;
    inline void addKeyboardInput(KeyboardCode keyboardKeyCodeInput, float modifier)
    {
        InputDescriptor desc;
        desc.type = InputType::Keyboard;
        desc.input.keyboardKeyCodeInput = keyboardKeyCodeInput;
        desc.modifier = modifier;
        descriptors.push_back(desc);
    }
    inline void addMouseInput(InputMouseType mouseTypeInput, float modifier)
    {
        InputDescriptor desc;
        desc.type = InputType::Mouse;
        desc.input.mouseInput = mouseTypeInput;
        desc.modifier = modifier;
        descriptors.push_back(desc);
    }
    inline void addGamepadButtonInput(int gamepadButtonInput, float modifier)
    {
        InputDescriptor desc;
        desc.type = InputType::GamepadButton;
        desc.input.gamepadButtonInput = gamepadButtonInput;
        desc.modifier = modifier;
        descriptors.push_back(desc);
    }
    inline void addGamepadAxisInput(int gamepadAxisInput, float modifier)
    {
        InputDescriptor desc;
        desc.type = InputType::GamepadAxis;
        desc.input.gamepadAxisInput = gamepadAxisInput;
        desc.modifier = modifier;
        descriptors.push_back(desc);
    }
};

class InputProvider
{
public:
    EXPORT InputProvider();
    EXPORT virtual ~InputProvider();

    EXPORT unsigned int addInput(InputDescriptorList &list, void *userData, void (*callback)(InputType type, InputData *data, float value, void *userData));
    EXPORT void removeInput(int index);
    EXPORT void removeAllInputs();

    EXPORT void addInputDetect(void (*callback)(InputType type));
    EXPORT void removeAllInputDetectors();

    EXPORT virtual void triggerInput(InputType type, InputData &data);

    static EXPORT void provideNewInput(InputType type, InputData &data);

    static EXPORT void setMousePosition(int x, int y, bool generateMoveEvents = false);

    // returns index you can use to remove it later
    EXPORT int addAnyKeyboardInput(void *userData, InputAnyCallback callback);
    EXPORT void removeAnyKeyboardInput(int index);

    EXPORT int addAnyMouseInput(void *userData, InputAnyCallback callback);
    EXPORT void removeAnyMouseInput(int index);

    EXPORT int addAnyGamepadInput(void *userData, InputAnyCallback callback);
    EXPORT void removeAnyGamepadInput(int index);

    EXPORT void addGamepad(Gamepad *gamepad);
    EXPORT void removeGamepad(Gamepad *gamepad);

protected:
    EXPORT bool isUsingGamepad(Gamepad *gamepad);
    void processAnyInput(InputType type, InputData &data);

    static std::vector<InputProvider *> inputs;

    static int prevMouseX;
    static int prevMouseY;

    std::vector<InputDescriptorCallbackData> descriptorCallbacks;
    std::vector<InputDetectorCallbackData> detectorCallbacks;

    std::vector<InputAny> anyKeyboardInputCallbacks;
    std::vector<InputAny> anyMouseInputCallbacks;
    std::vector<InputAny> anyGamepadInputCallbacks;

    std::vector<Gamepad *> gamepads;
};
