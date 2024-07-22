// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#define CONST_PI 3.1415926f
#define CONST_PI2 6.2831853f

#ifdef _WIN32
#define WINDOWS_ONLY
#endif

#ifdef __APPLE__
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT __declspec(dllexport)
#endif

#ifndef _strdup
#define _strdup strdup
#endif

enum class ColorMode
{
    Lit = 0,
    Alpha = 1,
    Addition = 2,
    Substraction = 3,
};

enum class AudioFormat
{
    UNKNOWN = 0,
    MONO_8,
    MONO_16,
    MONO_32,
    MONO_32F,
    STEREO_8,
    STEREO_16,
    STEREO_32,
    STEREO_32F
};

enum Extension
{
    UNKNOWN = 0,
    WAV,
    OGG
};
