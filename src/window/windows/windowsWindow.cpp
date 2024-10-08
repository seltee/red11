// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "settings.h"
#include "windowsWindow.h"
#include "data/inputProvider.h"
#include "windowsWindowUtils.h"
#include "red11.h"
#include <algorithm>
#include <shellscalingapi.h>
#include <windows.h>
#include <XInput.h>
#include <hidsdi.h>
#include <hidusage.h>
#include <hidpi.h>

#ifdef WINDOWS_ONLY
typedef BOOL(WINAPI *SetProcessDPIAwareFunc)();

// For XInput 1.3 (Windows 7 and above)
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "hid.lib")

#define CLASS_NAME L"Red11 Window Class"

WindowsWindow::WindowsWindow(const std::string &windowName, int width, int height, int flags)
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
    setProcessDPIAware();

    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register the window class.
    WNDCLASSW wc = {};
    ZeroMemory(&wc, sizeof(WNDCLASSW));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = windowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

    if (!RegisterClassW(&wc))
    {
        state.bIsValid = false;
        return;
    }

    std::wstring wideTitle;
    int convertResult = MultiByteToWideChar(CP_UTF8, 0, windowName.c_str(), windowName.size(), NULL, 0);
    if (convertResult <= 0)
    {
        wideTitle = L"Noname";
    }
    else
    {
        wideTitle.resize(convertResult + 10);
        convertResult = MultiByteToWideChar(CP_UTF8, 0, windowName.c_str(), (int)windowName.size(), &wideTitle[0], (int)wideTitle.size());
        if (convertResult <= 0)
        {
            wideTitle = L"Noname";
        }
    }

    int displayWidth = GetSystemMetrics(SM_CXSCREEN);
    int displayHeight = GetSystemMetrics(SM_CYSCREEN);

    // Get the work area (area excluding the taskbar)
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int visibleWidth = workArea.right - workArea.left;
    int visibleHeight = workArea.bottom - workArea.top;

    int positionX = state.bIsFullscreen ? 0 : (visibleWidth - (width + getAdditionalWindowWidth())) / 2;
    int positionY = state.bIsFullscreen ? 0 : (visibleHeight - (height + getAdditionalWindowHeight())) / 2;
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
        positionX, positionY, state.realWidth + getAdditionalWindowWidth(), state.realHeight + getAdditionalWindowHeight(),
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

    hCursorDefault = LoadCursor(NULL, IDC_ARROW);
    hCursorHand = LoadCursor(NULL, IDC_HAND);
    hCursorWait = LoadCursor(NULL, IDC_WAIT);
    hCursorCross = LoadCursor(NULL, IDC_CROSS);
    hCursorIBeam = LoadCursor(NULL, IDC_IBEAM);
    hCursorHelp = LoadCursor(NULL, IDC_HELP);

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);
    SetActiveWindow(hWnd);

    enableGamepadInput(hWnd);

    setCursorIcon(MouseCursorIcon::Default);
}

void WindowsWindow::processWindow()
{
    bCursorOverWindow = true;

    TRACKMOUSEEVENT tme = {sizeof(tme)};
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = hWnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&tme);

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
        // Get the work area (area excluding the taskbar)
        RECT workArea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
        int visibleWidth = workArea.right - workArea.left;
        int visibleHeight = workArea.bottom - workArea.top;

        int positionX = fullscreenState ? 0 : (visibleWidth - state.requesedWidth) / 2;
        int positionY = fullscreenState ? 0 : (visibleHeight - state.requesedHeight) / 2;
        int newRealWidth = state.bIsFullscreen ? displayWidth : state.requesedWidth + getAdditionalWindowWidth();
        int newRealHeight = state.bIsFullscreen ? displayHeight : state.requesedHeight + getAdditionalWindowHeight();

        SetWindowPos(hWnd, HWND_TOP, positionX, positionY, newRealWidth, newRealHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        ShowWindow(hWnd, true);
        UpdateWindow(hWnd);
    }
}

void WindowsWindow::setResolution(unsigned int width, unsigned int height)
{
    state.requesedWidth = width;
    state.requesedHeight = height;

    int displayWidth = GetSystemMetrics(SM_CXSCREEN);
    int displayHeight = GetSystemMetrics(SM_CYSCREEN);
    int positionX = state.bIsFullscreen ? 0 : (displayWidth - state.requesedWidth) / 2;
    int positionY = state.bIsFullscreen ? 0 : (displayHeight - state.requesedHeight) / 2;
    int newRealWidth = state.bIsFullscreen ? displayWidth : state.requesedWidth + getAdditionalWindowWidth();
    int newRealHeight = state.bIsFullscreen ? displayHeight : state.requesedHeight + getAdditionalWindowHeight();

    SetWindowPos(hWnd, HWND_TOP, positionX, positionY, newRealWidth, newRealHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);
}

std::vector<VideoMode> WindowsWindow::getVideoModesList(unsigned int limitToFps, bool sortByWidth)
{
    DEVMODE devMode;
    int modeNum = 0;

    // Initialize the DEVMODE structure to 0
    ZeroMemory(&devMode, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);

    std::vector<VideoMode> videoModes;

    // EnumDisplaySettings returns 0 when there are no more settings to enumerate
    while (EnumDisplaySettings(NULL, modeNum++, &devMode))
    {
        if (devMode.dmBitsPerPel == 32)
        {
            unsigned int width = static_cast<unsigned int>(devMode.dmPelsWidth);
            unsigned int height = static_cast<unsigned int>(devMode.dmPelsHeight);
            unsigned int refreshRate = static_cast<unsigned int>(devMode.dmDisplayFrequency);

            if ((limitToFps == 0 || limitToFps == devMode.dmDisplayFrequency) && !listHasMode(videoModes, width, height, refreshRate))
                videoModes.push_back(VideoMode({width, height, refreshRate}));
        }
    }

    if (sortByWidth)
    {
        std::sort(videoModes.begin(), videoModes.end(), [](const VideoMode &a, const VideoMode &b)
                  { return a.width == b.width ? a.height == b.height ? a.refreshRate < b.refreshRate : a.height < b.height : a.width < b.width; });
    }

    return videoModes;
}

std::vector<unsigned int> WindowsWindow::getRefreshRatesList()
{
    DEVMODE devMode;
    int modeNum = 0;

    // Initialize the DEVMODE structure to 0
    ZeroMemory(&devMode, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);

    std::vector<unsigned int> refreshRates;

    // EnumDisplaySettings returns 0 when there are no more settings to enumerate
    while (EnumDisplaySettings(NULL, modeNum++, &devMode))
    {
        if (devMode.dmBitsPerPel == 32)
        {
            unsigned int refreshRate = static_cast<unsigned int>(devMode.dmDisplayFrequency);
            if (!listHasRefreshRate(refreshRates, refreshRate))
                refreshRates.push_back(refreshRate);
        }
    }

    std::sort(refreshRates.begin(), refreshRates.end(), [](const int &a, const int &b)
              { return a < b; });

    return refreshRates;
}

std::vector<unsigned int> WindowsWindow::getRefreshRatesListForResolution(unsigned int width, unsigned int height)
{
    DEVMODE devMode;
    int modeNum = 0;

    // Initialize the DEVMODE structure to 0
    ZeroMemory(&devMode, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);

    std::vector<unsigned int> refreshRates;

    // EnumDisplaySettings returns 0 when there are no more settings to enumerate
    while (EnumDisplaySettings(NULL, modeNum++, &devMode))
    {
        if (devMode.dmBitsPerPel == 32)
        {
            unsigned int screenWidth = static_cast<unsigned int>(devMode.dmPelsWidth);
            unsigned int screenHeight = static_cast<unsigned int>(devMode.dmPelsHeight);
            unsigned int refreshRate = static_cast<unsigned int>(devMode.dmDisplayFrequency);

            if (screenWidth == width && screenHeight == height && !listHasRefreshRate(refreshRates, refreshRate))
                refreshRates.push_back(refreshRate);
        }
    }

    std::sort(refreshRates.begin(), refreshRates.end(), [](const int &a, const int &b)
              { return a < b; });

    return refreshRates;
}

bool WindowsWindow::isResolutionAvailable(unsigned int width, unsigned int height, bool bIsInFullscreen)
{
    DEVMODE devMode;
    int modeNum = 0;

    // Initialize the DEVMODE structure to 0
    ZeroMemory(&devMode, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);

    std::vector<unsigned int> refreshRates;

    // EnumDisplaySettings returns 0 when there are no more settings to enumerate
    while (EnumDisplaySettings(NULL, modeNum++, &devMode))
    {
        if (devMode.dmBitsPerPel == 32)
        {
            unsigned int screenWidth = static_cast<unsigned int>(devMode.dmPelsWidth);
            unsigned int screenHeight = static_cast<unsigned int>(devMode.dmPelsHeight);

            if (bIsInFullscreen)
            {
                if (width == screenWidth && height == screenHeight)
                    return true;
            }
            else
            {
                if (width <= screenWidth && height <= screenHeight)
                    return true;
            }
        }
    }

    return false;
}

bool WindowsWindow::isResolutionAvailable(unsigned int width, unsigned int height, unsigned int refreshRate, bool bIsInFullscreen)
{
    DEVMODE devMode;
    int modeNum = 0;

    // Initialize the DEVMODE structure to 0
    ZeroMemory(&devMode, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);

    std::vector<unsigned int> refreshRates;

    // EnumDisplaySettings returns 0 when there are no more settings to enumerate
    while (EnumDisplaySettings(NULL, modeNum++, &devMode))
    {
        if (devMode.dmBitsPerPel == 32 && static_cast<unsigned int>(devMode.dmDisplayFrequency) == refreshRate)
        {
            unsigned int screenWidth = static_cast<unsigned int>(devMode.dmPelsWidth);
            unsigned int screenHeight = static_cast<unsigned int>(devMode.dmPelsHeight);

            if (bIsInFullscreen)
            {
                if (width == screenWidth && height == screenHeight)
                    return true;
            }
            else
            {
                if (width <= screenWidth && height <= screenHeight)
                    return true;
            }
        }
    }

    return false;
}

void WindowsWindow::setMousePosition(int x, int y, bool generateMoveEvents)
{
    if (bIsFocused && (mousePositionX != x || mousePositionY != y))
    {
        POINT p = {x, y};
        ClientToScreen(hWnd, &p);
        if (SetCursorPos(p.x, p.y))
        {
            InputProvider::setMousePosition(x, y, generateMoveEvents);

            this->mousePositionX = p.x;
            this->mousePositionY = p.y;
        }
    }
}

void WindowsWindow::setMousePosition(const MousePosition &position, bool generateMoveEvents)
{
    setMousePosition(position.x, position.y);
}

void WindowsWindow::updateMousePosition(int x, int y)
{
    mousePositionX = x;
    mousePositionY = y;
}

void WindowsWindow::updateMouseOverWindow(bool bState)
{
    this->bCursorOverWindow = bState;
    if (!bState)
        setCursorIcon(MouseCursorIcon::Default);
}

MousePosition WindowsWindow::getMousePosition()
{
    return MousePosition({mousePositionX, mousePositionY});
}

void WindowsWindow::setCursorVisibility(bool state)
{
    if (bCursorShown != state)
    {
        bCursorShown = state;
        ShowCursor(state);
    }
}

bool WindowsWindow::isFocused()
{
    return bIsFocused;
}

int WindowsWindow::getStyleForState(const WindowState &state)
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

void WindowsWindow::setIsFocused(bool bState)
{
    bIsFocused = bState;
}

float WindowsWindow::getAdditionalWindowWidth()
{
    int additionalWindowWidth = GetSystemMetrics(SM_CYFRAME) * 2 + 16;
    return static_cast<float>(additionalWindowWidth);
}

float WindowsWindow::getAdditionalWindowHeight()
{
    int additionalWindowHeight = GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION) + 16;
    return static_cast<float>(additionalWindowHeight);
}

bool WindowsWindow::isCursorOverWindow()
{
    return bCursorOverWindow;
}

void WindowsWindow::setCursorIcon(MouseCursorIcon icon, bool bForce)
{
    if (mouseIcon != icon || bForce)
    {
        mouseIcon = icon;
        switch (icon)
        {
        case MouseCursorIcon::None:
        case MouseCursorIcon::Default:
            SetCursor(hCursorDefault);
            break;
        case MouseCursorIcon::Hand:
            SetCursor(hCursorHand);
            break;
        case MouseCursorIcon::Wait:
            SetCursor(hCursorWait);
            break;
        case MouseCursorIcon::Cross:
            SetCursor(hCursorCross);
            break;
        case MouseCursorIcon::IBeam:
            SetCursor(hCursorIBeam);
            break;
        case MouseCursorIcon::Help:
            SetCursor(hCursorHelp);
            break;
        }
    }
}

void WindowsWindow::resetCursorIcon()
{
    setCursorIcon(mouseIcon, true);
}

std::vector<Gamepad *> WindowsWindow::getGamepadList()
{
    std::vector<Gamepad *> list;
    // Get the number of input devices
    unsigned int deviceCount = 0;
    GetRawInputDeviceList(nullptr, &deviceCount, sizeof(RAWINPUTDEVICELIST));

    if (deviceCount == 0)
        return list;

    // Allocate memory for the device list
    RAWINPUTDEVICELIST *deviceList = new RAWINPUTDEVICELIST[deviceCount];
    GetRawInputDeviceList(deviceList, &deviceCount, sizeof(RAWINPUTDEVICELIST));

    // Iterate over all devices
    for (unsigned int i = 0; i < deviceCount; ++i)
    {
        // Check if the device is a gamepad (or joystick)
        if (deviceList[i].dwType == RIM_TYPEHID)
        {
            RID_DEVICE_INFO rdi;
            rdi.cbSize = sizeof(rdi);
            unsigned int rdiSize = sizeof(rdi);
            GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &rdiSize);

            // Check for gamepad based on usage page and usage ID
            if (rdi.hid.usUsagePage == 0x01 && (rdi.hid.usUsage == 0x04 || rdi.hid.usUsage == 0x05))
            {
                Gamepad *gamepad = getGamepad(deviceList[i].hDevice);
                list.push_back(gamepad);
            }
        }
    }
    return list;
}

Gamepad *WindowsWindow::getGamepad(HANDLE handle)
{
    for (auto &item : gamepadList)
    {
        if (item->getHandle() == handle)
            return item;
    }
    WindowsGamepad *gamepad = new WindowsGamepad(handle);
    gamepadList.push_back(gamepad);
    return gamepad;
}

void WindowsWindow::setProcessDPIAware()
{
    HMODULE user32 = LoadLibrary("user32.dll");
    if (user32 == NULL)
    {
        printf("Failed to load user32.dll");
        return;
    }

    // Get the address of SetProcessDPIAware
    SetProcessDPIAwareFunc SetProcessDPIAware = (SetProcessDPIAwareFunc)GetProcAddress(user32, "SetProcessDPIAware");

    if (SetProcessDPIAware)
        SetProcessDPIAware();

    // Free the library
    FreeLibrary(user32);
}

bool WindowsWindow::listHasMode(std::vector<VideoMode> &modes, unsigned int width, unsigned int height, unsigned int refreshRate)
{
    for (auto &mode : modes)
    {
        if (mode.width == width && mode.height == height && mode.refreshRate == refreshRate)
            return true;
    }
    return false;
}

bool WindowsWindow::listHasRefreshRate(std::vector<unsigned int> &refreshRates, unsigned int refreshRate)
{
    for (auto &rate : refreshRates)
    {
        if (rate == refreshRate)
            return true;
    }
    return false;
}

#endif