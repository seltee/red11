// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include "utils/utils.h"
#include "utils/math.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <locale>
#include <codecvt>

float randf()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float randf(float from, float to)
{
    return from + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (to - from)));
}

void calcTangets(const float *v1, const float *v2, const float *v3, float *out)
{
    // calculate tangetns and bitangents
    Vector3 edge1(v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]);
    Vector3 edge2(v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]);
    Vector2 deltaUV1(v2[6] - v1[6], v2[7] - v1[7]);
    Vector2 deltaUV2(v3[6] - v1[6], v3[7] - v1[7]);

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    out[0] = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    out[1] = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    out[2] = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    out[3] = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    out[4] = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    out[5] = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
}

int getFileByteSize(std::string path)
{
    struct stat stat_buf;
    int rc = stat(path.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string convertWCharToString(const wchar_t *wcharStr)
{
    if (!wcharStr)
        return "";

    // Use wstring_convert to convert wstring (wide string) to string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string str = converter.to_bytes(wcharStr);

    return str;
}
