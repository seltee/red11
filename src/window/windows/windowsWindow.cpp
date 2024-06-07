// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsWindow.h"
#include "data/inputProvider.h"
#include <shellscalingapi.h>
#include <windows.h>

#ifdef WINDOWS_ONLY

#define CLASS_NAME L"Red11 Window Class"

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        InputData keyDownData;
        keyDownData.keyboard.keyCode = (KeyboardCode)wParam;
        keyDownData.keyboard.state = 1;
        InputProvider::provideNewInput(InputType::Keyboard, keyDownData);
        break;

    case WM_KEYUP:
        InputData keyUpData;
        keyUpData.keyboard.keyCode = (KeyboardCode)wParam;
        keyUpData.keyboard.state = 0;
        InputProvider::provideNewInput(InputType::Keyboard, keyUpData);
        break;

    case WM_MOUSEMOVE:
        InputData mouseData;
        mouseData.mouse.type = InputMouseType::PositionX;
        mouseData.mouse.value = (int)(lParam & 0xffff);
        InputProvider::provideNewInput(InputType::Mouse, mouseData);
        mouseData.mouse.type = InputMouseType::PositionY;
        mouseData.mouse.value = (int)(lParam >> 16);
        InputProvider::provideNewInput(InputType::Mouse, mouseData);
        break;

    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    return 0;
}

WindowsWindow::WindowsWindow(const char *windowName, int width, int height, bool bIsFulltscreen)
{
    state.width = width;
    state.height = height;
    state.bIsFulltscreen = bIsFulltscreen;
    state.bIsValid = true;
    state.bIsCloseRequested = false;

    // HMODULE handle = LoadLibraryA(mod);
    // return handle && GetProcAddress(handle, func);
    SetProcessDPIAware();

    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register the window class.
    WNDCLASSW wc = {};
    ZeroMemory(&wc, sizeof(WNDCLASSW));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

    if (!RegisterClassW(&wc))
    {
        state.bIsValid = false;
        return;
    }

    std::wstring wideTitle;
    int convertResult = MultiByteToWideChar(CP_UTF8, 0, windowName, strlen(windowName), NULL, 0);
    if (convertResult <= 0)
    {
        wideTitle = L"Noname";
    }
    else
    {
        wideTitle.resize(convertResult + 10);
        convertResult = MultiByteToWideChar(CP_UTF8, 0, windowName, (int)strlen(windowName), &wideTitle[0], (int)wideTitle.size());
        if (convertResult <= 0)
        {
            wideTitle = L"Noname";
        }
    }

    // CreateWindowHandle
    hwnd = CreateWindowExW(
        0,                   // Optional window styles.
        CLASS_NAME,          // Window class
        wideTitle.c_str(),   // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if (hwnd == NULL)
    {
        state.bIsValid = false;
        return;
    }

    ShowWindow(hwnd, true);
    UpdateWindow(hwnd);
}

void WindowsWindow::processWindow()
{
    if (state.bIsValid && !state.bIsCloseRequested)
    {

        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
            {
                state.bIsCloseRequested = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void WindowsWindow::setMousePosition(int x, int y, bool generateMoveEvents)
{
    POINT p = {x, y};
    ClientToScreen(hwnd, &p);
    if (SetCursorPos(p.x, p.y)){
        InputProvider::setMousePosition(x, y, generateMoveEvents);
    }

}

#endif