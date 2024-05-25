// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "mesh.h"

Mesh::Mesh(void *verticies, VertexDataType type, int vLength, PolygonTriPoints *polygons, int pLength)
{
    this->type = type;
    static unsigned int lastIndex = 0;
    index = lastIndex;
    lastIndex++;

    if (type == VertexDataType::PositionNormal)
    {
        this->verticies.vertexPosition = new VertexDataPosition[vLength];
        this->vLength = vLength;
        for (int i = 0; i < vLength; i++)
            this->verticies.vertexPosition[i] = ((VertexDataPosition *)verticies)[i];
    }
    if (type == VertexDataType::PositionColor)
    {
        this->verticies.vertexPositionColor = new VertexDataColored[vLength];
        this->vLength = vLength;
        for (int i = 0; i < vLength; i++)
            this->verticies.vertexPositionColor[i] = ((VertexDataColored *)verticies)[i];
    }

    this->polygons = new PolygonTriPoints[pLength];
    this->pLength = pLength;
    for (int i = 0; i < pLength; i++)
        this->polygons[i] = polygons[i];
}

Mesh::~Mesh()
{
    if (type == VertexDataType::PositionNormal && this->verticies.vertexPosition)
        delete this->verticies.vertexPosition;
    if (type == VertexDataType::PositionColor && this->verticies.vertexPositionColor)
        delete this->verticies.vertexPositionColor;

    if (this->polygons)
        delete[] this->polygons;
}
