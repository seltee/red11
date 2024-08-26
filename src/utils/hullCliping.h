// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "physics/collisionManifold.h"

#define MAX_VERTS 1024

class HullCliping
{
public:
    static int clipFace(const Vector3 *pVtxIn, int numVertsIn, Vector3 &planeNormalWS, float planeEqWS, Vector3 *ppVtxOut);

    static int clipFaceAgainstHull(
        const Vector3 &separatingNormal,
        HullPolygon *polygonsA,
        int polygonsAmountA,
        Vector3 *verticiesA,
        int verticiesAmountA,
        Vector3 *normalsA,
        Vector3 *mutVerticies,
        int mutVerticiesAmount,
        const float minDist,
        const float maxDist,
        Vector4 *contactsOut,
        int contactCapacity);

    static int clipHullAgainstHull(const Vector3 &separatingNormal,
                                   HullPolygon *polygonsA,
                                   int polygonsAmountA,
                                   Vector3 *verticiesA,
                                   int verticiesAmountA,
                                   Vector3 *normalsA,
                                   HullPolygon *polygonsB,
                                   int polygonsAmountB,
                                   Vector3 *verticiesB,
                                   int verticiesAmountB,
                                   Vector3 *normalsB,
                                   const float minDist,
                                   float maxDist,
                                   Vector4 *contactsOut,
                                   int contactCapacity);

    static void clipHullAgainstHull(
        Vector3 sepNormal,
        HullPolygon *polygonsA,
        int polygonsAmountA,
        Vector3 *verticiesA,
        int verticiesAmountA,
        Vector3 *normalsA,
        HullPolygon *polygonsB,
        int polygonsAmountB,
        Vector3 *verticiesB,
        int verticiesAmountB,
        Vector3 *normalsB,
        CollisionManifold *manifold);

    static void clipHullAgainstHullWithPostTransformation(
        Vector3 sepNormal,
        HullPolygon *polygonsA,
        int polygonsAmountA,
        Vector3 *verticiesA,
        int verticiesAmountA,
        Vector3 *normalsA,
        HullPolygon *polygonsB,
        int polygonsAmountB,
        Vector3 *verticiesB,
        int verticiesAmountB,
        Vector3 *normalsB,
        Matrix4 *model,
        CollisionManifold *manifold);
};
