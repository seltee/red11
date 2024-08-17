// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "spline.h"
#include <red11.h>

Spline::Spline(const Vector3 *points, int nPointsAmount, SplineInterpolation interpolation, bool bIsLooped)
{
    if (interpolation == SplineInterpolation::Native)
        interpolation = SplineInterpolation::Linear;

    if (nPointsAmount < 2)
    {
        Red11::getLogger()->logFileAndConsole("Unable to create spline with less than 2 points\n");
        return;
    }

    this->points = new Vector3[nPointsAmount];
    this->nPointsAmount = nPointsAmount;
    this->bIsLooped = bIsLooped;
    this->interpolation = interpolation;

    memcpy(this->points, points, sizeof(Vector3) * nPointsAmount);

    rebuildSegments();
}

void Spline::rebuildSegments()
{
    if (splineSegments)
        delete[] splineSegments;

    nSegmentsAmount = bIsLooped ? nPointsAmount : nPointsAmount - 1;
    splineSegments = new SplineSegment[nSegmentsAmount];
    memset(splineSegments, 0, sizeof(SplineSegment) * nSegmentsAmount);

    this->fSplineLength = 0.0f;
    for (int s = 0; s < nSegmentsAmount; s++)
    {
        int next = (s + 1) % nPointsAmount;
        const Vector3 &v1 = points[s];
        const Vector3 &v2 = points[next];
        splineSegments[s].fLength = glm::length(v1 - v2);
        splineSegments[s].a = s;
        splineSegments[s].b = next;
        this->fSplineLength += splineSegments[s].fLength;
    }
}

Vector3 Spline::getPointOnSpline(float fDistance)
{
    if (nPointsAmount == 0)
        return Vector3(0.0f);

    if (fDistance < 0.0f)
        return this->points[0];

    if (fDistance >= fSplineLength)
        return this->points[bIsLooped ? 0 : nPointsAmount - 1];

    float fWalkedDistance = 0.0f;
    for (int s = 0; s < nSegmentsAmount; s++)
    {
        const SplineSegment &segment = splineSegments[s];
        if (fWalkedDistance + segment.fLength > fDistance)
        {
            // point is on this segment
            float fSegmentDistance = fDistance - fWalkedDistance;
            float fOnSegment = fSegmentDistance / splineSegments[s].fLength;

            if (interpolation == SplineInterpolation::Linear)
            {
                return this->points[segment.a] * (1.0f - fOnSegment) + this->points[segment.b] * fOnSegment;
            }
            if (interpolation == SplineInterpolation::Smooth)
            {
                int p0 = segment.a - 1;
                if (p0 < 0)
                {
                    if (bIsLooped)
                        p0 += nPointsAmount;
                    else
                        p0 = segment.a;
                }
                int p3 = segment.b + 1;
                if (p3 >= nPointsAmount)
                {
                    if (bIsLooped)
                        p3 -= nPointsAmount;
                    else
                        p3 = segment.b;
                }
                return catmullRomSpline(this->points[p0], this->points[segment.a], this->points[segment.b], this->points[p3], fOnSegment);
            }
            return Vector3(0.0f);
        }
        else
        {
            fWalkedDistance += segment.fLength;
        }
    }
    return Vector3(0.0f);
}