// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "mesh.h"

Mesh::Mesh(VertexDataType type, void *verticies, int vLength, PolygonTriPoints *polygons, int pLength, Matrix4 *transformation)
{
    this->type = type;
    this->vLength = vLength;
    static unsigned int lastIndex = 0;
    index = lastIndex;
    lastIndex++;

    if (type == VertexDataType::PositionUV)
    {
        this->verticies.vertexPositionUV = new VertexDataUV[vLength];
        for (int i = 0; i < vLength; i++)
            if (transformation)
            {
                this->verticies.vertexPositionUV[i] = ((VertexDataUV *)verticies)[i];
                this->verticies.vertexPositionUV[i].position = Vector3(*transformation * Vector4(this->verticies.vertexPositionUV[i].position, 1.0f));
            }
            else
                this->verticies.vertexPositionUV[i] = ((VertexDataUV *)verticies)[i];
    }
    if (type == VertexDataType::PositionColor)
    {
        this->verticies.vertexPositionColor = new VertexDataColored[vLength];
        for (int i = 0; i < vLength; i++)
            if (transformation)
            {
                this->verticies.vertexPositionColor[i] = ((VertexDataColored *)verticies)[i];
                this->verticies.vertexPositionColor[i].position = Vector3(*transformation * Vector4(this->verticies.vertexPositionColor[i].position, 1.0f));
            }
            else
                this->verticies.vertexPositionColor[i] = ((VertexDataColored *)verticies)[i];
    }

    this->polygons = new PolygonTriPoints[pLength];
    this->pLength = pLength;
    for (int i = 0; i < pLength; i++)
        this->polygons[i] = polygons[i];
}

Mesh::~Mesh()
{
    if (type == VertexDataType::PositionUV && this->verticies.vertexPositionUV)
        delete this->verticies.vertexPositionUV;
    if (type == VertexDataType::PositionColor && this->verticies.vertexPositionColor)
        delete this->verticies.vertexPositionColor;
    if (this->polygons)
        delete[] this->polygons;
}

void Mesh::addDeform(Deform *deform)
{
    deform->index = deforms.size();
    deforms.push_back(deform);
}