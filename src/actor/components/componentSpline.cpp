// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentSpline.h"

ComponentSpline::ComponentSpline()
{
}

ComponentSpline::~ComponentSpline()
{
}

void ComponentSpline::onRenderQueue(Renderer *renderer)
{
    if (spline && bShowDebugSpline)
    {
        const Matrix4 &transformation = getModelMatrix();
        const float step = 0.05f;

        for (float r = 0.0; r < spline->getLength(); r += step)
        {
            Vector3 pA = (Vector3)(transformation * Vector4(spline->getPointOnSpline(r), 1.0f));
            Vector3 pB = (Vector3)(transformation * Vector4(spline->getPointOnSpline(r + step), 1.0f));

            renderer->queueLine(pA, pB, Color(0.2f, 0.8f, 0.2f));
        }
    }
}

void ComponentSpline::setSpline(Spline *spline)
{
    this->spline = spline;
}
