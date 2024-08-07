// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "hullCliping.h"
#include "reduceContacts.h"

// Clips a face to the back of a plane, return the number of vertices out, stored in ppVtxOut
int HullCliping::clipFace(const Vector3 *pVtxIn, int numVertsIn, Vector3 &planeNormalWS, float planeEqWS, Vector3 *ppVtxOut)
{
    int ve;
    float ds, de;
    int numVertsOut = 0;
    if (numVertsIn < 2)
        return 0;

    Vector3 firstVertex = pVtxIn[numVertsIn - 1];
    Vector3 endVertex = pVtxIn[0];

    ds = glm::dot(planeNormalWS, firstVertex) + planeEqWS;

    for (ve = 0; ve < numVertsIn; ve++)
    {
        endVertex = pVtxIn[ve];

        de = glm::dot(planeNormalWS, endVertex) + planeEqWS;

        if (ds < 0)
        {
            if (de < 0)
            {
                // Start < 0, end < 0, so output endVertex
                ppVtxOut[numVertsOut++] = endVertex;
            }
            else
            {
                // Start < 0, end >= 0, so output intersection
                ppVtxOut[numVertsOut++] = lerp(firstVertex, endVertex, (ds * 1.0f / (ds - de)));
            }
        }
        else
        {
            if (de < 0)
            {
                // Start >= 0, end < 0 so output intersection and end
                ppVtxOut[numVertsOut++] = lerp(firstVertex, endVertex, (ds * 1.0f / (ds - de)));
                ppVtxOut[numVertsOut++] = endVertex;
            }
        }
        firstVertex = endVertex;
        ds = de;
    }
    return numVertsOut;
}

int HullCliping::clipFaceAgainstHull(
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
    int contactCapacity)
{
    int numContactsOut = 0;
    int numVertsIn = mutVerticiesAmount;
    int numVertsOut = 0;

    Vector3 *pVtxIn = mutVerticies;
    Vector3 vBuff[MAX_POINTS];
    Vector3 *pVtxOut = vBuff;

    const HullPolygon *closestFaceA = nullptr;

    float dmin = FLT_MAX;
    for (int a = 0; a < polygonsAmountA; a++)
    {
        float d = glm::dot(normalsA[a], separatingNormal);
        if (d < dmin)
        {
            dmin = d;
            closestFaceA = &polygonsA[a];
        }
    }

    if (closestFaceA == nullptr)
        return numContactsOut;

    int numVerticesA = closestFaceA->pointsAmount;
    for (int e0 = 0; e0 < numVerticesA; e0++)
    {
        const Vector3 a = verticiesA[closestFaceA->points[e0]];
        const Vector3 b = verticiesA[closestFaceA->points[(e0 + 1) % numVerticesA]];

        const Vector3 worldEdge = a - b;
        const Vector3 worldPlaneAnormal = normalsA[closestFaceA->index];

        Vector3 planeNormalWS = -glm::cross(worldEdge, worldPlaneAnormal);
        float planeEqWS = -glm::dot(a, planeNormalWS);

        numVertsOut = clipFace(pVtxIn, numVertsIn, planeNormalWS, planeEqWS, pVtxOut);

        Vector3 *tmp = pVtxOut;
        pVtxOut = pVtxIn;
        pVtxIn = tmp;
        numVertsIn = numVertsOut;
        numVertsOut = 0;
    }

    // Keep points that are behind the witness face
    Vector3 planeNormalWS = normalsA[closestFaceA->index];
    float planeEqWS = -glm::dot(planeNormalWS, verticiesA[closestFaceA->points[0]]);

    for (int i = 0; i < numVertsIn; i++)
    {
        float depth = glm::dot(planeNormalWS, pVtxIn[i]) + planeEqWS;

        if (depth <= minDist)
        {
            depth = minDist;
        }
        if (numContactsOut < contactCapacity)
        {
            if (depth <= maxDist)
            {
                Vector3 pointInWorld = pVtxIn[i];
                contactsOut[numContactsOut++] = Vector4(pointInWorld, depth);
            }
        }
        else
        {
            printf("exceeding contact capacity (%d,%df)\n", numContactsOut, contactCapacity);
        }
    }

    return numContactsOut;
}

int HullCliping::clipHullAgainstHull(const Vector3 &separatingNormal,
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
                                     int contactCapacity)
{
    int numContactsOut = 0;
    const HullPolygon *closestFaceB = nullptr;
    float dmax = -FLT_MAX;

    for (int b = 0; b < polygonsAmountB; b++)
    {
        Vector3 &polyWorldNormal = normalsB[b];
        float d = glm::dot(polyWorldNormal, separatingNormal);
        if (d > dmax)
        {
            dmax = d;
            closestFaceB = &polygonsB[b];
        }
    }

    if (closestFaceB != nullptr)
    {
        Vector3 mutVerticies[MAX_POINTS];
        int mutVerticiesAmount = closestFaceB->pointsAmount;
        for (int i = 0; i < mutVerticiesAmount; i++)
            mutVerticies[i] = verticiesB[closestFaceB->points[i]];

        numContactsOut = clipFaceAgainstHull(separatingNormal,
                                             polygonsA,
                                             polygonsAmountA,
                                             verticiesA,
                                             verticiesAmountA,
                                             normalsA,
                                             mutVerticies,
                                             mutVerticiesAmount,
                                             minDist,
                                             maxDist,
                                             contactsOut,
                                             contactCapacity);
    }

    return numContactsOut;
}

void HullCliping::clipHullAgainstHull(
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
    CollisionManifold *manifold)
{
    Vector4 contactsOut[MAX_POINTS];
    const int contactCapacity = MAX_POINTS;

    const float minDist = -1.0f;
    const float maxDist = 0.0f;

    int numContactsOut = clipHullAgainstHull(
        sepNormal,
        polygonsA,
        polygonsAmountA,
        verticiesA,
        verticiesAmountA,
        normalsA,
        polygonsB,
        polygonsAmountB,
        verticiesB,
        verticiesAmountB,
        normalsB,
        minDist,
        maxDist,
        contactsOut,
        contactCapacity);

    if (numContactsOut > 0)
    {
        Vector3 normalOnSurfaceB = -sepNormal;

        int contacts4[4] = {0, 1, 2, 3};
        int numPoints = reduceContacts(contactsOut, numContactsOut, normalOnSurfaceB, contacts4);

        Vector3 middle(0.0f);
        float depth = 0.0f;
        for (int p = 0; p < numPoints; p++)
        {
            depth = fminf(contactsOut[contacts4[p]].w, depth);
            middle += Vector3(contactsOut[contacts4[p]]) / (float)numPoints;
        }
        manifold->addCollisionPoint(middle, middle, -depth, normalOnSurfaceB);
    }
}

void HullCliping::clipHullAgainstHullWithPostTransformation(
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
    CollisionManifold *manifold)
{
    Vector4 contactsOut[MAX_POINTS];
    const int contactCapacity = MAX_POINTS;

    const float minDist = -1.0f;
    const float maxDist = 0.0f;

    int numContactsOut = clipHullAgainstHull(
        sepNormal,
        polygonsA,
        polygonsAmountA,
        verticiesA,
        verticiesAmountA,
        normalsA,
        polygonsB,
        polygonsAmountB,
        verticiesB,
        verticiesAmountB,
        normalsB,
        minDist,
        maxDist,
        contactsOut,
        contactCapacity);

    if (numContactsOut > 0)
    {
        Vector3 normalOnSurfaceB = -sepNormal;

        int contacts4[4] = {0, 1, 2, 3};
        int numPoints = reduceContacts(contactsOut, numContactsOut, normalOnSurfaceB, contacts4);

        Vector3 middle(0.0f);
        float depth = 0.0f;

        Matrix3 rotationScaleMatrix = Matrix3(*model);
        float scaleX = glm::length(rotationScaleMatrix[0]);
        float scaleY = glm::length(rotationScaleMatrix[1]);
        float scaleZ = glm::length(rotationScaleMatrix[2]);
        float scale = fminf(fminf(scaleX, scaleY), scaleZ);

        for (int p = 0; p < numPoints; p++)
        {
            depth = fminf(contactsOut[contacts4[p]].w, depth);
            Vector3 out = Vector3(*model * Vector4(Vector3(contactsOut[contacts4[p]]), 1.0f));
            middle += out / (float)numPoints;
        }

        Vector3 normal = glm::normalize(rotationScaleMatrix * normalOnSurfaceB);

        manifold->addCollisionPoint(middle, middle, -depth * scale, normal);
    }
}