// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "utils/utils.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>

class Logger
{
public:
    EXPORT Logger(std::string filePath);
    EXPORT void logFile(const char *format, ...);
    EXPORT void logConsole(const char *format, ...);
    EXPORT void logFileAndConsole(const char *format, ...);

    inline bool isFileWritingEnabled() { return bIsFileWritingEnabled; }

private:
    bool bIsFileWritingEnabled = false;
    FILE *pLogFile = nullptr;

    std::string formatString(const char *format, va_list arg);
    std::string convert(unsigned int num, int base);
};
