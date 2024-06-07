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

void InputProvider::addInput(InputDescriptorList &list, void *userData, void (*callback)(InputType type, InputData *data, float value, void *userData))
{
    descriptorCallbacks.push_back({list.descriptors, userData, callback});
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
                    if (it.input.gamepadButtonInput == data.gamepadButton.keyCode)
                    {
                        descriptor.callback(type, &data, float(data.gamepadButton.state ? 1.0f : 0.0f) * it.modifier, descriptor.userData);
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