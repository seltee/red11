// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/logger.h"
#include "logger.h"
#include <string>

enum class OSType
{
    Windows
};

struct CPUInfo
{
    std::string name;
    unsigned int coreAmount = 0;
};

struct GPUInfo
{
    std::string name;
    unsigned int vendorId;
    unsigned int deviceId;
};

struct RAMInfo
{
    unsigned int totalAmount;
    unsigned int totalVirtualAmount;
};

struct OSInfo
{
    OSType type;
    std::string name;
    std::string fullName;
    std::string version;
    unsigned int majorVersionNumber;
    unsigned int minorVersionNumber;
    unsigned int buildNumber;
};

struct SystemInfo
{
    std::string name;
    std::string manufacturer;
    std::string model;
};

class SysInfo
{
public:
    EXPORT SysInfo();

    EXPORT void logIntoFile(Logger *logger, bool logSameToConsole = true);

    inline CPUInfo &getCPUInfo() { return cpuInfo; }
    inline GPUInfo &getGPUInfo() { return gpuInfo; }
    inline RAMInfo &getRAMInfo() { return ramInfo; }
    inline OSInfo &getOSInfo() { return osInfo; }
    inline SystemInfo &getSystemInfo() { return systemInfo; }

    EXPORT unsigned int getAvailableRam();
    EXPORT unsigned int getAvailableVirtualRam();

protected:
    CPUInfo fillCPUInfo();
    GPUInfo fillGPUInfo();
    RAMInfo fillRAMInfo();
    OSInfo fillOSInfo();
    SystemInfo fillSystemInfo();

    CPUInfo cpuInfo;
    GPUInfo gpuInfo;
    RAMInfo ramInfo;
    OSInfo osInfo;
    SystemInfo systemInfo;
};
