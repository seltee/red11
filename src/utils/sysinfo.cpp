// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "sysinfo.h"

#ifdef WINDOWS_ONLY
#include <windows.h>
#include <wbemidl.h>
#include <comdef.h>
#include <iostream>
#include <d3d9.h>

#pragma comment(lib, "wbemuuid.lib")

extern "C" NTSTATUS NTAPI RtlGetVersion(POSVERSIONINFOEXW lpVersionInformation);
typedef NTSTATUS(WINAPI *RtlGetVersionPtr)(POSVERSIONINFOEXW);

#endif

#include <vector>

SysInfo::SysInfo()
{
    cpuInfo = fillCPUInfo();
    gpuInfo = fillGPUInfo();
    ramInfo = fillRAMInfo();
    osInfo = fillOSInfo();
    systemInfo = fillSystemInfo();
};

void SysInfo::logIntoFile(Logger *logger, bool logSameToConsole)
{
    using LogFunc = void (Logger::*)(const char *format, ...);
    LogFunc logFunc = logSameToConsole ? &Logger::logFileAndConsole : &Logger::logFile;

    (logger->*logFunc)("CPU: %s (threads number: %i)", cpuInfo.name.c_str(), cpuInfo.coreAmount);
    (logger->*logFunc)("GPU: %s", gpuInfo.name.c_str());
    (logger->*logFunc)("Ram total: %i", ramInfo.totalAmount);
    (logger->*logFunc)("Virtual Ram total: %i", ramInfo.totalVirtualAmount);
    (logger->*logFunc)("OS: %s (%i.%i) build %i", osInfo.fullName.c_str(), osInfo.majorVersionNumber, osInfo.minorVersionNumber, osInfo.buildNumber);
    (logger->*logFunc)("System: %s", systemInfo.name.c_str());

}

unsigned int SysInfo::getAvailableRam()
{
    unsigned int amount = 0;
#ifdef WINDOWS_ONLY
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    amount = statex.ullAvailPhys / (1024 * 1024);
#endif
    return amount;
}

EXPORT unsigned int SysInfo::getAvailableVirtualRam()
{
    unsigned int amount = 0;
#ifdef WINDOWS_ONLY
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    amount = statex.ullAvailVirtual / (1024 * 1024);
#endif
    return amount;
}

CPUInfo SysInfo::fillCPUInfo()
{
    CPUInfo info;
    memset(&info, 0, sizeof(CPUInfo));

#ifdef WINDOWS_ONLY
    char CPUBrandString[0x80];
    int CPUInfoVal[4] = {-1};
    __cpuid(CPUInfoVal, 0x80000000);
    unsigned int nExIds = CPUInfoVal[0];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    // Get the information associated with each extended ID.
    for (unsigned int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfoVal, i);

        // Interpret CPU brand string and cache information.
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfoVal, sizeof(CPUInfoVal));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfoVal, sizeof(CPUInfoVal));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfoVal, sizeof(CPUInfoVal));
    }

    info.name = std::string(CPUBrandString);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.coreAmount = sysInfo.dwNumberOfProcessors;
#endif

    return info;
}

GPUInfo SysInfo::fillGPUInfo()
{
    GPUInfo info;
    memset(&info, 0, sizeof(GPUInfo));

#ifdef WINDOWS_ONLY
    IDirect3D9 *d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d)
    {
        D3DADAPTER_IDENTIFIER9 adapterInfo;
        if (SUCCEEDED(d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterInfo)))
        {
            info.name = std::string(adapterInfo.Description);
            info.deviceId = adapterInfo.DeviceId;
            info.vendorId = adapterInfo.VendorId;
        }
        d3d->Release();
    }
#endif

    return info;
}

RAMInfo SysInfo::fillRAMInfo()
{
    RAMInfo info;
    memset(&info, 0, sizeof(RAMInfo));

#ifdef WINDOWS_ONLY
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    info.totalAmount = statex.ullTotalPhys / (1024 * 1024);
    info.totalVirtualAmount = statex.ullTotalVirtual / (1024 * 1024);
#endif
    return info;
}

OSInfo SysInfo::fillOSInfo()
{
    OSInfo info;
    memset(&info, 0, sizeof(OSInfo));

#ifdef WINDOWS_ONLY
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    bool ntddllSuccess = false;
    if (hMod)
    {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
        if (RtlGetVersion)
        {
            info.type = OSType::Windows;
            info.name = std::string("Windows");
            info.fullName = std::string("Windows");

            OSVERSIONINFOEXW osInfo = {0};
            osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
            RtlGetVersion(&osInfo);

            info.majorVersionNumber = osInfo.dwMajorVersion;
            info.minorVersionNumber = osInfo.dwMinorVersion;
            info.buildNumber = osInfo.dwBuildNumber;
            info.version = std::to_string(osInfo.dwMajorVersion) + "." + std::to_string(osInfo.dwMinorVersion);

            if (osInfo.dwMajorVersion == 5 && osInfo.dwMinorVersion == 1)
                info.fullName = "Windows XP";
            else if (osInfo.dwMajorVersion == 5 && osInfo.dwMinorVersion == 2)
                info.fullName = "Windows XP Professional x64 Edition";
            else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 0)
                info.fullName = "Windows Vista";
            else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 1)
                info.fullName = "Windows 7";
            else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 2)
                info.fullName = "Windows 8";
            else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 3)
                info.fullName = "Windows 8.1";
            else if (osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0 && osInfo.dwBuildNumber >= 22000)
                info.fullName = "Windows 11";
            else if (osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0)
                info.fullName = "Windows 10";
            else
                info.fullName = "Unknown Windows version";

            ntddllSuccess = true;
        }
    }

    if (!ntddllSuccess)
    {
        info.type = OSType::Windows;
        info.name = "Windows";
        info.fullName = "Windows";

        OSVERSIONINFO osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        if (GetVersionEx(&osvi))
        {
            info.majorVersionNumber = osvi.dwMajorVersion;
            info.minorVersionNumber = osvi.dwMinorVersion;
            info.buildNumber = osvi.dwBuildNumber;
            info.version = std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion);
        }
    }

#endif

    return info;
}

SystemInfo SysInfo::fillSystemInfo()
{
    SystemInfo info;
    memset(&info, 0, sizeof(SystemInfo));
    info.name = std::string("");
    info.manufacturer = std::string("Unknown Manufacturer");
    info.model = std::string("Unknown Model");

#ifdef WINDOWS_ONLY
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    IWbemLocator *pLoc = NULL;
    CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);

    IWbemServices *pSvc = NULL;
    pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);

    CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    IEnumWbemClassObject *pEnumerator = NULL;
    pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_ComputerSystem"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;
        pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
        info.manufacturer = convertWCharToString(vtProp.bstrVal);
        VariantClear(&vtProp);

        pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
        info.model = convertWCharToString(vtProp.bstrVal);
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

#endif

    info.name = info.manufacturer + " " + info.model;
    return info;
}