// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshDescriptor.h"

MeshDescriptor::MeshDescriptor()
{
}

MeshDescriptor::~MeshDescriptor()
{
    if (vertexes)
        delete[] vertexes;
    if (normals)
        delete[] normals;
    if (uvs)
        delete[] uvs;
    if (uvIndexes)
        delete[] uvIndexes;
    if (polygonIndexes)
        delete[] polygonIndexes;
    if (outVertex)
        delete[] outVertex;
    if (outPolygons)
        delete[] outPolygons;
}

void MeshDescriptor::provideVertex(double *list, int countOfDoubles)
{
    if (vertexes)
        delete[] vertexes;

    vertexAmount = countOfDoubles / 3;
    vertexes = new Vector3[vertexAmount];

    for (int i = 0; i < vertexAmount; i++)
    {
        int s = i * 3;
        vertexes[i].x = list[s];
        vertexes[i].y = list[s + 1];
        vertexes[i].z = list[s + 2];
    }

    dirty = true;
}

void MeshDescriptor::providePolygonIndexes(int *list, int countOfIndexes)
{
    polygonIndexes = new int[countOfIndexes];
    memcpy(polygonIndexes, list, sizeof(int) * countOfIndexes);
    polygonIndexesAmount = countOfIndexes;

    dirty = true;
}

void MeshDescriptor::provideUVData(double *list, int countOfDoubles)
{
    uvsAmount = countOfDoubles / 2;
    uvs = new Vector2[uvsAmount];

    for (int i = 0; i < uvsAmount; i++)
    {
        int shift = i * 2;
        uvs[i].x = list[shift];
        uvs[i].y = 1.0f - list[shift + 1];
    }

    dirty = true;
}

void MeshDescriptor::provideUVIndexes(int *list, int countOfIndexes)
{
    uvIndexes = new int[countOfIndexes];
    memcpy(uvIndexes, list, sizeof(int) * countOfIndexes);
    uvIndexesAmount = countOfIndexes;

    dirty = true;
}

void MeshDescriptor::provideNormals(double *list, int countOfDoubles)
{
    if (normals)
        delete[] normals;

    normalsAmount = countOfDoubles / 3;
    normals = new Vector3[normalsAmount];

    for (int i = 0; i < vertexAmount; i++)
    {
        int s = i * 3;
        normals[i].x = (float)list[s];
        normals[i].y = (float)list[s + 1];
        normals[i].z = (float)list[s + 2];
    }

    dirty = true;
}

void MeshDescriptor::rebuild()
{
    if (dirty && vertexes && normals && uvs && uvIndexes)
    {
        dirty = false;

        std::vector<VertexDataUV> vecOutVertexes;
        std::vector<PolygonTriPoints> vecOutPolygons;

        std::vector<PolygonTriPointsUV> vecPolygons;
        PolygonTriPointsUV p;
        p.a = p.b = p.c = 0.0f;
        int filled = 0;
        for (int i = 0; i < polygonIndexesAmount; i++)
        {
            int index = polygonIndexes[i];
            int uvIndex = uvIndexes[i];

            bool reset = false;
            if (index < 0)
            {
                index = ~index;
                reset = true;
            }

            if (filled == 0)
            {
                p.a = index;
                p.UVa = uvIndex;
                filled++;
            }
            else if (filled == 1)
            {
                p.b = index;
                p.UVb = uvIndex;
                filled++;
            }
            else if (filled == 2)
            {
                p.c = index;
                p.UVc = uvIndex;
                filled++;
                vecPolygons.push_back(p);
            }
            else
            {
                p.b = p.a;
                p.UVb = p.UVa;
                p.a = p.c;
                p.UVa = p.UVc;
                p.c = index;
                p.UVc = uvIndex;

                int sw = p.b;
                int UVsv = p.UVb;
                p.b = p.c;
                p.UVb = p.UVc;
                p.c = sw;
                p.UVc = UVsv;

                vecPolygons.push_back(p);
            }

            if (reset)
                filled = 0;
        }

        unsigned int polygonsAmount = vecPolygons.size();

        for (int i = 0; i < polygonsAmount; i++)
        {
            unsigned int indexA = vecPolygons.at(i).b;
            unsigned int indexB = vecPolygons.at(i).a;
            unsigned int indexC = vecPolygons.at(i).c;

            unsigned int indexUVA = vecPolygons.at(i).UVb;
            unsigned int indexUVB = vecPolygons.at(i).UVa;
            unsigned int indexUVC = vecPolygons.at(i).UVc;

            VertexDataUV vecOutVertexA = VertexDataUV(vertexes[indexA], uvs[indexUVA], normals[indexA]);
            VertexDataUV vecOutVertexB = VertexDataUV(vertexes[indexB], uvs[indexUVB], normals[indexB]);
            VertexDataUV vecOutVertexC = VertexDataUV(vertexes[indexC], uvs[indexUVC], normals[indexC]);

            PolygonTriPoints tri;
            tri.a = getIndex(vecOutVertexA, &vecOutVertexes);
            tri.b = getIndex(vecOutVertexB, &vecOutVertexes);
            tri.c = getIndex(vecOutVertexC, &vecOutVertexes);
            vecOutPolygons.push_back(tri);
        }

        if (outVertex)
            delete[] outVertex;

        if (outPolygons)
            delete[] outPolygons;

        outVertexAmount = vecOutVertexes.size();
        outVertex = new VertexDataUV[outVertexAmount];

        outPolygonsAmount = vecOutPolygons.size();
        outPolygons = new PolygonTriPoints[outPolygonsAmount];

        for (int i = 0; i < outVertexAmount; i++)
            outVertex[i] = vecOutVertexes.at(i);
        for (int i = 0; i < outPolygonsAmount; i++)
            outPolygons[i] = vecOutPolygons.at(i);
    }
}

unsigned int MeshDescriptor::getIndex(VertexDataUV &newVertex, std::vector<VertexDataUV> *vecOutVertexes)
{
    int len = vecOutVertexes->size();
    for (int index = 0; index < len; index++)
    {
        if (vecOutVertexes->at(index).position.x == newVertex.position.x &&
            vecOutVertexes->at(index).position.y == newVertex.position.y &&
            vecOutVertexes->at(index).position.z == newVertex.position.z &&
            vecOutVertexes->at(index).normal.x == newVertex.normal.x &&
            vecOutVertexes->at(index).normal.y == newVertex.normal.y &&
            vecOutVertexes->at(index).normal.z == newVertex.normal.z &&
            vecOutVertexes->at(index).uv.x == newVertex.uv.x &&
            vecOutVertexes->at(index).uv.y == newVertex.uv.y)
        {
            return index;
        }
    }
    vecOutVertexes->push_back(newVertex);
    return vecOutVertexes->size() - 1;
}
