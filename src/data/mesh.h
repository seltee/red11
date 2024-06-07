// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include <string>

#define MAX_MESH_COUNT 100000

enum class VertexDataType
{
    PositionUV,
    PositionColor
};

struct VertexDataUV
{
    Vector3 position;
    Vector2 uv;
    Vector3 normal;

    VertexDataUV()
    {
        this->position = Vector3(0.0f);
        this->uv = Vector2(0.0f);
        this->normal = Vector3(0.0f);
    }

    VertexDataUV(Vector3 position, Vector2 uv, Vector3 normal)
    {
        this->position = position;
        this->uv = uv;
        this->normal = normal;
    }

    VertexDataUV(float px, float py, float pz, float u, float v, float nx, float ny, float nz)
    {
        this->position = Vector3(px, py, pz);
        this->uv = Vector2(u, v);
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
    void *ptr;
    VertexDataUV *vertexPositionUV;
    VertexDataColored *vertexPositionColor;
};

inline int getVertexDataTypeSize(VertexDataType type)
{
    if (type == VertexDataType::PositionUV)
        return sizeof(VertexDataUV);
    if (type == VertexDataType::PositionColor)
        return sizeof(VertexDataColored);
    return 0;
}

class Mesh
{
public:
    Mesh(VertexDataType type, void *verticies, int vLength, PolygonTriPoints *polygons, int pLength, Matrix4 *transformation = nullptr);
    ~Mesh();

    inline unsigned int getIndex() { return index; }

    inline int getPolygonsAmount() { return pLength; }
    inline int getVerticiesAmount() { return vLength; };

    inline PolygonTriPoints *getPolygons() { return polygons; };
    inline VertexData *getVerticies() { return &verticies; };

    inline VertexDataType getType() { return type; }

protected:
    VertexDataType type = VertexDataType::PositionUV;
    VertexData verticies;
    int vLength = 0;
    PolygonTriPoints *polygons = nullptr;
    int pLength = 0;

    unsigned int index;
};
