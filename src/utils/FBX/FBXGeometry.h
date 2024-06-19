// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include "data/mesh.h"
#include "data/deform.h"
#include "utils/utils.h"
#include "FBXNode.h"
#include "FBXDeform.h"

struct PolygonUV
{
    Vector2 uv1;
    Vector2 uv2;
    Vector2 uv3;
};

struct PolygonTriPointsUV
{
    unsigned int a, b, c;
    unsigned int UVa, UVb, UVc;
};

class FBXGeometry
{
public:
    EXPORT FBXGeometry(FBXNode *node);
    EXPORT ~FBXGeometry();

    EXPORT Mesh *getMesh();

    EXPORT void addDeformer(FBXDeform *deform);

    EXPORT bool hasDeformer(FBXDeform *deform);

    inline std::vector<FBXDeform *> *getDeforms() { return &deforms; }

    inline VertexDataUV *getVertexes()
    {
        rebuild();
        return outVertex;
    }
    inline int getVertexAmount()
    {
        rebuild();
        return outVertexAmount;
    }
    inline PolygonTriPoints *getPolygons()
    {
        rebuild();
        return outPolygons;
    }
    inline int getPolygonsAmount()
    {
        rebuild();
        return outPolygonsAmount;
    }

    unsigned long long id;

protected:
    EXPORT void processDeformToMesh(Mesh *mesh, FBXDeform *deform);

    EXPORT void provideVertex(double *list, int countOfDoubles);
    EXPORT void providePolygonIndexes(int *list, int countOfIndexes);
    EXPORT void provideUVData(double *list, int countOfDoubles);
    EXPORT void provideUVIndexes(int *list, int countOfIndexes);
    EXPORT void provideNormals(double *list, int countOfDoubles);

    void rebuild();
    unsigned int getIndex(VertexDataUV &newVertex, std::vector<VertexDataUV> *vecOutVertexes);
    bool dirty = true;

    Vector3 *vertexes = nullptr;
    int vertexAmount = 0;

    Vector3 *normals = nullptr;
    int normalsAmount = 0;

    Vector2 *uvs = nullptr;
    int uvsAmount = 0;

    VertexDataUV *outVertex = nullptr;
    int outVertexAmount = 0;

    PolygonTriPoints *outPolygons = nullptr;
    int outPolygonsAmount = 0;

    int *uvIndexes = nullptr;
    int uvIndexesAmount = 0;

    int *polygonIndexes = nullptr;
    int polygonIndexesAmount = 0;

    Mesh *mesh = nullptr;

    std::vector<FBXDeform *> deforms;
};