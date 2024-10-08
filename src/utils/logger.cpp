// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "logger.h"
#include <stdio.h>

Logger::Logger(const std::string &filePath)
{
    mutex.lock();
    pLogFile = fopen(filePath.c_str(), "w");
    if (pLogFile != nullptr)
        bIsFileWritingEnabled = true;
    mutex.unlock();
}

void Logger::logFile(const char *format, ...)
{
    mutex.lock();
    if (bIsFileWritingEnabled)
    {
        va_list arg;
        va_start(arg, format);

        std::string printString = formatString(format, arg);
        fputs((printString + "\n").c_str(), pLogFile);
    }
    mutex.unlock();
}

void Logger::logConsole(const char *format, ...)
{
    mutex.lock();
    va_list arg;
    va_start(arg, format);

    std::string printString = formatString(format, arg);
    puts(printString.c_str());
    mutex.unlock();
}

void Logger::logFileAndConsole(const char *format, ...)
{
    mutex.lock();
    va_list arg;
    va_start(arg, format);

    std::string printString = formatString(format, arg);
    puts(printString.c_str());
    if (bIsFileWritingEnabled)
    {
        fputs((printString + "\n").c_str(), pLogFile);
    }
    mutex.unlock();
}

std::string Logger::formatString(const char *format, va_list arg)
{

    std::string out = "";
    const char *iterator = format;
    char *s;
    int i;
    double f;
    unsigned long lu;
    unsigned long long llu;

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

                case 'l':
                    if (*(iterator + 1) == 'u')
                    {
                        lu = va_arg(arg, unsigned long); // Fetch unsigned long
                        out += std::to_string(lu);
                        iterator++;
                    }
                    else if (*(iterator + 1) == 'l' && *(iterator + 2) == 'u')
                    {
                        llu = va_arg(arg, unsigned long long); // Fetch unsigned long long
                        out += std::to_string(llu);
                        iterator += 2;
                    }
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