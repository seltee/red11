// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "windowsWindow.h"
#include "data/inputProvider.h"
#include <shellscalingapi.h>
#include <windows.h>

#ifdef WINDOWS_ONLY

#define CLASS_NAME L"Red11 Window Class"

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowsWindow *window = (WindowsWindow *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    PWINDOWPOS winpos;
    RECT rect;

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

    case WM_WINDOWPOSCHANGED:
        winpos = (PWINDOWPOS)lParam;
        GetClientRect(hWnd, &rect);
        window->udpateRealSize(rect.right, rect.bottom);
        break;

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return 0;
}

WindowsWindow::WindowsWindow(const char *windowName, int width, int height, int flags)
{
    state.requesedWidth = width;
    state.requesedHeight = height;
    state.bIsFullscreen = (flags & WINDOW_FULLSCREEN) != 0;
    state.bIsBorderless = (flags & WINDOW_BORDERLESS) != 0;
    state.bIsResizable = (flags & WINDOW_RESIZABLE) != 0;
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

    int displayWidth = GetSystemMetrics(SM_CXSCREEN);
    int displayHeight = GetSystemMetrics(SM_CYSCREEN);

    int positionX = state.bIsFullscreen ? 0 : (displayWidth - width) / 2;
    int positionY = state.bIsFullscreen ? 0 : (displayHeight - height) / 2;
    state.realWidth = state.bIsFullscreen ? displayWidth : width;
    state.realHeight = state.bIsFullscreen ? displayHeight : height;

    int style = getStyleForState(state);

    // CreateWindowHandle
    hWnd = CreateWindowExW(
        0,                 // Optional window styles.
        CLASS_NAME,        // Window class
        wideTitle.c_str(), // Window text
        style,             // Window style
        // position and size
        positionX, positionY, state.realWidth, state.realHeight,
        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        this       // Additional application data
    );

    if (hWnd == NULL)
    {
        state.bIsValid = false;
        return;
    }

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);
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

void WindowsWindow::setFullscreen(bool fullscreenState)
{
    if (state.bIsFullscreen != fullscreenState)
    {
        state.bIsFullscreen = fullscreenState;

        int style = getStyleForState(state);
        SetWindowLong(hWnd, GWL_STYLE, style);

        int displayWidth = GetSystemMetrics(SM_CXSCREEN);
        int displayHeight = GetSystemMetrics(SM_CYSCREEN);
        int positionX = fullscreenState ? 0 : (displayWidth - state.requesedWidth) / 2;
        int positionY = fullscreenState ? 0 : (displayHeight - state.requesedHeight) / 2;
        int newRealWidth = state.bIsFullscreen ? displayWidth : state.requesedWidth;
        int newRealHeight = state.bIsFullscreen ? displayHeight : state.requesedHeight;

        SetWindowPos(hWnd, HWND_TOP, positionX, positionY, newRealWidth, newRealHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        ShowWindow(hWnd, true);
        UpdateWindow(hWnd);
    }
}

void WindowsWindow::setResolution(int width, int height)
{
    state.requesedWidth = width;
    state.requesedHeight = height;

    int displayWidth = GetSystemMetrics(SM_CXSCREEN);
    int displayHeight = GetSystemMetrics(SM_CYSCREEN);
    int positionX = state.bIsFullscreen ? 0 : (displayWidth - state.requesedWidth) / 2;
    int positionY = state.bIsFullscreen ? 0 : (displayHeight - state.requesedHeight) / 2;
    int newRealWidth = state.bIsFullscreen ? displayWidth : state.requesedWidth;
    int newRealHeight = state.bIsFullscreen ? displayHeight : state.requesedHeight;

    SetWindowPos(hWnd, HWND_TOP, positionX, positionY, newRealWidth, newRealHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);
}

void WindowsWindow::setMousePosition(int x, int y, bool generateMoveEvents)
{
    POINT p = {x, y};
    ClientToScreen(hWnd, &p);
    if (SetCursorPos(p.x, p.y))
    {
        InputProvider::setMousePosition(x, y, generateMoveEvents);
    }
}

int WindowsWindow::getStyleForState(WindowState &state)
{
    int style = WS_OVERLAPPEDWINDOW;
    if (state.bIsFullscreen)
    {
        style = WS_POPUP | WS_VISIBLE;
    }
    else if (state.bIsBorderless)
    {
        style = WS_POPUP | WS_VISIBLE;
    }
    else if (state.bIsResizable)
    {
        style = WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_VISIBLE;
    }
    return style;
}

#endif