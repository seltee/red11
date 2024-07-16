// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "logger.h"
#include <stdio.h>

Logger::Logger(std::string filePath)
{
    pLogFile = fopen(filePath.c_str(), "w");
    if (pLogFile != nullptr)
        bIsFileWritingEnabled = true;
}

void Logger::logFile(const char *format, ...)
{
    if (bIsFileWritingEnabled)
    {
        va_list arg;
        va_start(arg, format);

        std::string printString = formatString(format, arg);
        fputs((printString + "\n").c_str(), pLogFile);
    }
}

void Logger::logConsole(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);

    std::string printString = formatString(format, arg);
    puts(printString.c_str());
}

void Logger::logFileAndConsole(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);

    std::string printString = formatString(format, arg);
    puts(printString.c_str());
    if (bIsFileWritingEnabled)
    {
        fputs((printString + "\n").c_str(), pLogFile);
    }
}

std::string Logger::formatString(const char *format, va_list arg)
{

    std::string out = "";
    const char *iterator = format;
    char *s;
    int i;
    double f;

    for (; *iterator != 0; iterator++)
    {
        if (*iterator == '%')
        {
            iterator++;
            if (*iterator)
            {
                switch (*iterator)
                {
                    // Fetch character
                case 'c':
                    i = va_arg(arg, int);
                    out += (char)i;
                    break;

                // Fetch Decimal/Integer argument
                case 'i':
                case 'd':
                    i = va_arg(arg, long int);
                    if (i < 0)
                    {
                        i = -i;
                        out += '-';
                    }
                    out += convert(i, 10);
                    break;

                    // Fetch Octal representation
                case 'o':
                    i = va_arg(arg, unsigned int);
                    out += convert(i, 8);
                    break;

                    // Fetch Hexadecimal representation
                case 'x':
                    i = va_arg(arg, unsigned int);
                    out = convert(i, 16);
                    break;

                    // Fetch String
                case 's':
                    s = va_arg(arg, char *); // Fetch string
                    out += s;
                    break;

                    // Fetch float/double
                case 'f':
                    f = va_arg(arg, double); // Fetch double
                    out += std::to_string(f);
                    break;

                default:
                    printf("Unknown format argument %c%c", '%', *iterator);
                    break;
                }
            }
        }
        else
        {
            out += *iterator;
        }
    }

    va_end(arg);
    return out;
}

std::string Logger::convert(unsigned int num, int base)
{
    static char Representation[] = "0123456789ABCDEF";
    std::string out;

    do
    {
        out = Representation[num % base] + out;
        num /= base;
    } while (num != 0);

    return out;
}