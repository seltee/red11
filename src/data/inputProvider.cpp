// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "inputProvider.h"

#define DEF_MOUSE -99999

std::vector<InputProvider *> InputProvider::inputs;
int InputProvider::prevMouseX = DEF_MOUSE;
int InputProvider::prevMouseY = DEF_MOUSE;

InputProvider::InputProvider()
{
    inputs.push_back(this);
}

InputProvider::~InputProvider()
{
    auto it = inputs.begin();
    while (it != inputs.end())
    {
        if ((*it) == this)
        {
            inputs.erase(it);
            break;
        }
        ++it;
    }
}

// Returns Id of the input
// Remove later this input by this Id if needed
unsigned int InputProvider::addInput(InputDescriptorList &list, void *userData, void (*callback)(InputType type, InputData *data, float value, void *userData))
{
    static unsigned int lastIndex = 0;
    unsigned int index = lastIndex;
    lastIndex++;

    descriptorCallbacks.push_back({index, list.descriptors, userData, callback});
    return index;
}

void InputProvider::removeInput(int index)
{
    for (auto it = descriptorCallbacks.begin(); it != descriptorCallbacks.end(); it++)
    {
        if (it->index == index)
        {
            descriptorCallbacks.erase(it);
            return;
        }
    }
}

void InputProvider::removeAllInputs()
{
    descriptorCallbacks.clear();
}

void InputProvider::addInputDetect(void (*callback)(InputType type))
{
    detectorCallbacks.push_back({callback});
}

void InputProvider::removeAllInputDetectors()
{
    detectorCallbacks.clear();
}

void InputProvider::triggerInput(InputType type, InputData &data)
{
    processAnyInput(type, data);
    for (auto &descriptor : descriptorCallbacks)
    {
        for (auto &it : descriptor.descriptors)
        {
            if (it.type == type)
            {
                switch (type)
                {
                case InputType::Keyboard:
                    if (it.input.keyboardKeyCodeInput == data.keyboard.keyCode)
                    {
                        descriptor.callback(type, &data, (data.keyboard.state ? 1.0f : 0.0f) * it.modifier, descriptor.userData);
                    }
                    break;
                case InputType::Mouse:
                    if (it.input.mouseInput == data.mouse.type)
                    {
                        descriptor.callback(type, &data, float(data.mouse.value) * it.modifier, descriptor.userData);
                    }
                    break;
                case InputType::GamepadButton:
                    if (isUsingGamepad(data.gamepadButton.gamepad))
                    {
                        if (it.input.gamepadButtonInput == data.gamepadButton.keyCode)
                        {
                            descriptor.callback(type, &data, float(data.gamepadButton.state ? 1.0f : 0.0f) * it.modifier, descriptor.userData);
                        }
                    }
                    break;
                case InputType::GamepadAxis:
                    if (it.input.gamepadAxisInput == data.gamepadAxis.axisCode)
                    {
                        descriptor.callback(type, &data, float(data.gamepadAxis.value) * it.modifier, descriptor.userData);
                    }
                    break;
                }
            }
        }
    }

    for (auto &it : detectorCallbacks)
    {
        it.callback(type);
    }
}

void InputProvider::provideNewInput(InputType type, InputData &data)
{
    // X mouse position is based on prev X mouse position
    if (type == InputType::Mouse && data.mouse.type == InputMouseType::PositionX)
    {
        if (prevMouseX != DEF_MOUSE)
        {
            InputData mouseData;
            mouseData.mouse.type = InputMouseType::MoveX;
            mouseData.mouse.value = data.mouse.value - prevMouseX;
            provideNewInput(InputType::Mouse, mouseData);
        }
        prevMouseX = data.mouse.value;
    }

    // Y mouse position is based on prev Y mouse position
    if (type == InputType::Mouse && data.mouse.type == InputMouseType::PositionY)
    {
        if (prevMouseY != DEF_MOUSE)
        {
            InputData mouseData;
            mouseData.mouse.type = InputMouseType::MoveY;
            mouseData.mouse.value = data.mouse.value - prevMouseY;
            provideNewInput(InputType::Mouse, mouseData);
        }
        prevMouseY = data.mouse.value;
    }

    for (auto &it : inputs)
    {
        it->triggerInput(type, data);
    }
}

void InputProvider::setMousePosition(int x, int y, bool generateMoveEvents)
{
    if (generateMoveEvents)
    {
        InputData mouseData;
        mouseData.mouse.type = InputMouseType::MoveX;
        mouseData.mouse.value = x - prevMouseX;
        provideNewInput(InputType::Mouse, mouseData);
        mouseData.mouse.type = InputMouseType::MoveY;
        mouseData.mouse.value = y - prevMouseY;
        provideNewInput(InputType::Mouse, mouseData);
    }

    prevMouseX = x;
    prevMouseY = y;
}

int InputProvider::addAnyKeyboardInput(void *userData, InputAnyCallback callback)
{
    static int lastIndex = 0;
    int index = lastIndex;
    lastIndex++;
    anyKeyboardInputCallbacks.push_back({callback, userData, index});
    return index;
}

void InputProvider::removeAnyKeyboardInput(int index)
{
    for (auto it = anyKeyboardInputCallbacks.begin(); it != anyKeyboardInputCallbacks.end(); it++)
    {
        if ((*it).index == index)
        {
            anyKeyboardInputCallbacks.erase(it);
            return;
        }
    }
}

int InputProvider::addAnyMouseInput(void *userData, InputAnyCallback callback)
{
    static int lastIndex = 0;
    int index = lastIndex;
    lastIndex++;
    anyMouseInputCallbacks.push_back({callback, userData, index});
    return index;
}

void InputProvider::removeAnyMouseInput(int index)
{
    for (auto it = anyMouseInputCallbacks.begin(); it != anyMouseInputCallbacks.end(); it++)
    {
        if ((*it).index == index)
        {
            anyMouseInputCallbacks.erase(it);
            return;
        }
    }
}

int InputProvider::addAnyGamepadInput(void *userData, InputAnyCallback callback)
{
    static int lastIndex = 0;
    int index = lastIndex;
    lastIndex++;
    anyGamepadInputCallbacks.push_back({callback, userData, index});
    return index;
}

void InputProvider::removeAnyGamepadInput(int index)
{
    for (auto it = anyGamepadInputCallbacks.begin(); it != anyGamepadInputCallbacks.end(); it++)
    {
        if ((*it).index == index)
        {
            anyGamepadInputCallbacks.erase(it);
            return;
        }
    }
}

void InputProvider::addGamepad(Gamepad *gamepad)
{
    gamepads.push_back(gamepad);
}

void InputProvider::removeGamepad(Gamepad *gamepad)
{
    for (auto it = gamepads.begin(); it != gamepads.end(); it++)
    {
        if ((*it)->is(gamepad))
        {
            gamepads.erase(it);
            return;
        }
    }
}
bool InputProvider::isUsingGamepad(Gamepad *gamepad)
{
    for (auto &it : gamepads)
    {
        if (it->is(gamepad))
        {
            return true;
        }
    }
    return false;
}

void InputProvider::processAnyInput(InputType type, InputData &data)
{
    if (type == InputType::Keyboard && data.keyboard.state)
    {
        for (auto &item : anyKeyboardInputCallbacks)
        {
            item.callback(item.userData);
        }
    }
    else if (type == InputType::Mouse &&
             data.mouse.type != InputMouseType::MoveX &&
             data.mouse.type != InputMouseType::MoveY &&
             data.mouse.type != InputMouseType::PositionX &&
             data.mouse.type != InputMouseType::PositionY && abs(data.mouse.value) > 0)
    {
        for (auto &item : anyMouseInputCallbacks)
        {
            item.callback(item.userData);
        }
    }
    else if (type == InputType::GamepadButton && data.gamepadButton.state && isUsingGamepad(data.gamepadButton.gamepad))
    {
        for (auto &item : anyGamepadInputCallbacks)
        {
            item.callback(item.userData);
        }
    }
    else if (type == InputType::GamepadAxis && fabsf(data.gamepadAxis.value) > 0.2f)
    {
        for (auto &item : anyGamepadInputCallbacks)
        {
            item.callback(item.userData);
        }
    }
}