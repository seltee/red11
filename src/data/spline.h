// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/algorithms.h"
#include <vector>

enum class SplineInterpolation
{
    Native,
    Linear,
    Smooth,
};

struct SplineSegment
{
    float fLength;
    int a, b;
};

class Spline
{
public:
    EXPORT Spline(const Vector3 *points, int nPointsAmount, SplineInterpolation interpolation = SplineInterpolation::Linear, bool bIsLooped = false);
    EXPORT void rebuildSegments();

    EXPORT Vector3 getPointOnSpline(float fDistance);

    inline float getLength() const { return fSplineLength; }

protected:
    inline Vector3 catmullRomSpline(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, float t)
    {
        return 0.5f * ((2.0f * p1) +
                       (-p0 + p2) * t +
                       (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
                       (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
    }

    float fSplineLength = 0.0f;

    Vector3 *points = nullptr;
    int nPointsAmount = 0;

    SplineSegment *splineSegments = nullptr;
    int nSegmentsAmount = 0;

    bool bIsLooped = false;
    SplineInterpolation interpolation = SplineInterpolation::Linear;
};