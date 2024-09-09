// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsWindowUtils.h"
#include "windowsWindow.h"
#include "data/inputProvider.h"
#include "red11.h"

#ifdef WINDOWS_ONLY

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowsWindow *window = (WindowsWindow *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    RECT rect;
    InputData inputData;

    if (message == WM_CREATE)
    {
        CREATESTRUCT *CreateStruct = (CREATESTRUCT *)lParam;
        window = (WindowsWindow *)CreateStruct->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
    }

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SETFOCUS:
        window->setIsFocused(true);
        break;

    case WM_KILLFOCUS:
        window->setIsFocused(false);
        break;

    case WM_KEYDOWN:
        inputData.keyboard.keyCode = (KeyboardCode)wParam;
        inputData.keyboard.state = 1;
        InputProvider::provideNewInput(InputType::Keyboard, inputData);
        break;

    case WM_KEYUP:
        inputData.keyboard.keyCode = (KeyboardCode)wParam;
        inputData.keyboard.state = 0;
        InputProvider::provideNewInput(InputType::Keyboard, inputData);
        break;

    case WM_SETCURSOR:
    {
        // Check if the cursor is within the client area
        if (LOWORD(lParam) == HTCLIENT)
            window->resetCursorIcon();
        break;
    }

    case WM_MOUSEMOVE:
        inputData.mouse.type = InputMouseType::PositionX;
        inputData.mouse.value = (int)(lParam & 0xffff);
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        inputData.mouse.type = InputMouseType::PositionY;
        inputData.mouse.value = (int)(lParam >> 16);
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        window->updateMousePosition((int)(lParam & 0xffff), (int)(lParam >> 16));
        break;

    case WM_LBUTTONDOWN:
        inputData.mouse.type = InputMouseType::LeftButton;
        inputData.mouse.value = 1.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_LBUTTONUP:
        inputData.mouse.type = InputMouseType::LeftButton;
        inputData.mouse.value = 0.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_RBUTTONDOWN:
        inputData.mouse.type = InputMouseType::RightButton;
        inputData.mouse.value = 1.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_RBUTTONUP:
        inputData.mouse.type = InputMouseType::RightButton;
        inputData.mouse.value = 0.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_MBUTTONDOWN:
        inputData.mouse.type = InputMouseType::MiddleButton;
        inputData.mouse.value = 1.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_MBUTTONUP:
        inputData.mouse.type = InputMouseType::MiddleButton;
        inputData.mouse.value = 0.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_MOUSEWHEEL:
        inputData.mouse.type = InputMouseType::Wheel;
        inputData.mouse.value = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / 120.0f;
        InputProvider::provideNewInput(InputType::Mouse, inputData);
        break;

    case WM_WINDOWPOSCHANGED:
        GetClientRect(hWnd, &rect);
        window->udpateRealSize(rect.right, rect.bottom);
        break;

    case WM_MOUSEHOVER:
        window->updateMouseOverWindow(true);
        break;

    case WM_MOUSELEAVE:
        window->updateMouseOverWindow(false);
        break;

    case WM_INPUT:
    {
        UINT dwSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
        if (dwSize == 0)
            break;
        LPBYTE lpb = new BYTE[dwSize];

        if (lpb == nullptr)
            break;

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
            Red11::getLogger()->logConsole("GetRawInputData does not return correct size!");

        RAWINPUT *raw = (RAWINPUT *)lpb;
        if (raw->header.dwType == RIM_TYPEHID)
        {
            // Get preparsed data
            PHIDP_PREPARSED_DATA pPreparsedData;
            UINT preparsedDataSize;
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, nullptr, &preparsedDataSize);
            pPreparsedData = (PHIDP_PREPARSED_DATA)malloc(preparsedDataSize);
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &preparsedDataSize);

            // Gamepad data
            RID_DEVICE_INFO rdi;
            rdi.cbSize = sizeof(rdi);
            unsigned int rdiSize = sizeof(rdi);
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICEINFO, &rdi, &rdiSize);

            Gamepad *gamepad = window->getGamepad(raw->header.hDevice);

            // Parse HID report using the preparsed data
            GamepadReport report = parseHidReport(raw->header.hDevice, pPreparsedData, (char *)raw->data.hid.bRawData, raw->data.hid.dwSizeHid, gamepad);
            for (int i = 0; i < MAX_GAMEPAD_BUTTONS; i++)
            {
                bool bIsChanged = gamepad->checkButtonStateChanged(i, report.buttons[i]);
                if (bIsChanged)
                {
                    inputData.gamepadButton.keyCode = i;
                    inputData.gamepadButton.state = report.buttons[i];
                    inputData.gamepadButton.gamepad = gamepad;
                    InputProvider::provideNewInput(InputType::GamepadButton, inputData);
                }
            }
            for (int i = 0; i < MAX_GAMEPAD_AXISES; i++)
            {
                bool bIsChanged = gamepad->checkAxisStateChanged(i, report.axises[i]);
                if (bIsChanged)
                {
                    inputData.gamepadAxis.axisCode = i;
                    inputData.gamepadAxis.value = report.axises[i];
                    inputData.gamepadAxis.gamepad = gamepad;
                    InputProvider::provideNewInput(InputType::GamepadAxis, inputData);
                }
            }
            free(pPreparsedData);
        }

        delete[] lpb;
        break;
    }

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return 0;
}

GamepadReport parseHidReport(HANDLE hDevice, PHIDP_PREPARSED_DATA pPreparsedData, char *report, UINT reportLength, Gamepad *gamepad)
{
    GamepadReport gamepadReport;
    memset(&gamepadReport, 0, sizeof(GamepadReport));

    HIDP_CAPS caps;
    HidP_GetCaps(pPreparsedData, &caps);

    // Example: Handling Button Inputs
    unsigned short numButtonCaps = caps.NumberInputButtonCaps;
    HIDP_BUTTON_CAPS buttonCaps[numButtonCaps];
    HidP_GetButtonCaps(HidP_Input, buttonCaps, &numButtonCaps, pPreparsedData);

    unsigned long numUsages = MAX_GAMEPAD_BUTTONS; // Adjust based on expected number of buttons
    unsigned short usageList[MAX_GAMEPAD_BUTTONS];
    HidP_GetUsages(HidP_Input, buttonCaps[0].UsagePage, 0, usageList, &numUsages, pPreparsedData, report, reportLength);

    for (unsigned long i = 0; i < numUsages; i++)
    {
        gamepadReport.buttons[usageList[i]] = true;
    }

    // Example: Handling Axis Inputs
    unsigned short numValueCaps = caps.NumberInputValueCaps;
    HIDP_VALUE_CAPS valueCaps[numValueCaps];
    HidP_GetValueCaps(HidP_Input, valueCaps, &numValueCaps, pPreparsedData);

    for (USHORT i = 0; i < numValueCaps; i++)
    {
        unsigned short usage = valueCaps[i].NotRange.Usage;
        unsigned short page = valueCaps[i].UsagePage;

        unsigned long value;
        HidP_GetUsageValue(HidP_Input, page, 0, usage, &value, pPreparsedData, report, reportLength);
        float fValue = fminf(fmaxf((static_cast<float>(value) / static_cast<float>(0xffff / 2)) - 1.0f, -1.0f), 1.0f);

        if (usage >= 0 && usage < MAX_GAMEPAD_AXISES && fabsf(fValue) > gamepad->getDeadZone())
            gamepadReport.axises[usage] = fValue;

        // DPAD
        if (usage == 57)
        {
            if (value == 1 || value == 2 || value == 8)
                gamepadReport.buttons[gamepad->getDefaultCodeDPadUp()] = true;
            if (value == 2 || value == 3 || value == 4)
                gamepadReport.buttons[gamepad->getDefaultCodeDPadRight()] = true;
            if (value == 4 || value == 5 || value == 6)
                gamepadReport.buttons[gamepad->getDefaultCodeDPadDown()] = true;
            if (value == 6 || value == 7 || value == 8)
                gamepadReport.buttons[gamepad->getDefaultCodeDPadLeft()] = true;
        }
    }

    return gamepadReport;
}

void enableGamepadInput(HWND hWnd)
{
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x05;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = hWnd;

    if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == false)
        Red11::getLogger()->logFileAndConsole("Failed to register raw input devices");
}

#endif