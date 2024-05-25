// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"

#define MAX_MESH_COUNT 100000

enum class VertexDataType
{
    PositionNormal,
    PositionColor
};

struct VertexDataPosition
{
    Vector3 position;
    Vector3 normal;

    VertexDataPosition()
    {
        this->position = Vector3(0.0f);
        this->normal = Vector3(0.0f);
    }

    VertexDataPosition(Vector3 position, Vector3 normal)
    {
        this->position = position;
        this->normal = normal;
    }

    VertexDataPosition(float px, float py, float pz, float nx, float ny, float nz)
    {
        this->position = Vector3(px, py, pz);
        this->normal = Vector3(nx, ny, nz);
    }
};

struct VertexDataColored
{
    Vector3 position;
    Vector3 normal;
    unsigned int color;

    VertexDataColored()
    {
        this->position = Vector3(0.0f);
        this->normal = Vector3(0.0f, 0.0f, 1.0f);
        this->color = 0;
    }

    VertexDataColored(float px, float py, float pz, float nx, float ny, float nz, unsigned int color)
    {
        this->position = Vector3(px, py, pz);
        this->normal = Vector3(normal);
        this->color = color;
    }

    VertexDataColored(Vector3 p, Vector3 n, unsigned int color)
    {
        this->position = p;
        this->normal = n;
        this->color = color;
    }
};

union VertexData
{
    VertexDataPosition *vertexPosition;
    VertexDataColored *vertexPositionColor;
};

class Mesh
{
public:
    Mesh(void *verticies, VertexDataType type, int vLength, PolygonTriPoints *polygons, int pLength);
    ~Mesh();

    inline unsigned int getIndex() { return index; }

    inline int getPolygonsAmount() { return pLength; }
    inline int getVerticiesAmount() { return vLength; };

    inline PolygonTriPoints *getPolygons() { return polygons; };
    inline VertexData getVerticies() { return verticies; };

    inline VertexDataType getType() { return type; }

protected:
    VertexDataType type = VertexDataType::PositionNormal;
    VertexData verticies;
    int vLength = 0;
    PolygonTriPoints *polygons = nullptr;
    int pLength = 0;

    unsigned int index;
};
