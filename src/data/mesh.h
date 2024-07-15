// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "data/deform.h"
#include "data/sphere.h"
#include <string>

#define MAX_MESH_COUNT 100000

enum class VertexDataType
{
    Unknown,
    PositionUV,
    PositionColor
};

struct VertexDataUV
{
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
    Vector3 tangent;
    Vector3 bitangent;
    int index;

    VertexDataUV()
    {
        this->index = 0;
        this->position = Vector3(0.0f);
        this->uv = Vector2(0.0f);
        this->normal = Vector3(0.0f);
        this->tangent = Vector3(0.0f);
        this->bitangent = Vector3(0.0f);
    }

    VertexDataUV(int index, Vector3 position, Vector2 uv, Vector3 normal)
    {
        this->index = index;
        this->position = position;
        this->uv = uv;
        this->normal = normal;
        this->tangent = Vector3(0.0f);
        this->bitangent = Vector3(0.0f);
    }

    VertexDataUV(int index, float px, float py, float pz, float u, float v, float nx, float ny, float nz)
    {
        this->index = index;
        this->position = Vector3(px, py, pz);
        this->uv = Vector2(u, v);
        this->normal = Vector3(nx, ny, nz);
        this->tangent = Vector3(0.0f);
        this->bitangent = Vector3(0.0f);
    }
};

struct VertexDataColored
{
    Vector3 position;
    Vector3 normal;
    unsigned int color;
    int index;

    VertexDataColored()
    {
        this->index = 0;
        this->position = Vector3(0.0f);
        this->normal = Vector3(0.0f, 0.0f, 1.0f);
        this->color = 0;
    }

    VertexDataColored(int index, float px, float py, float pz, float nx, float ny, float nz, unsigned int color)
    {
        this->index = index;
        this->position = Vector3(px, py, pz);
        this->normal = Vector3(normal);
        this->color = color;
    }

    VertexDataColored(int index, Vector3 p, Vector3 n, unsigned int color)
    {
        this->index = index;
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

    void addDeform(Deform *deform);
    inline Deform *getDeformByName(std::string &name)
    {
        for (auto &deform : deforms)
        {
            if (deform->isName(name))
                return deform;
        }
        return nullptr;
    }

    inline unsigned int getIndex() { return index; }

    inline int getPolygonsAmount() { return pLength; }
    inline int getVerticiesAmount() { return vLength; };

    inline PolygonTriPoints *getPolygons() { return polygons; };
    inline VertexData *getVerticies() { return &verticies; };
    inline std::vector<Deform *> *getDeforms() { return &deforms; }
    inline VertexDataType getType() { return type; }

    inline Vector4 getCentroid() { return centroid; }

    inline Sphere &getBoundVolumeSphere() { return boundVolume; }

    inline void setCastsShadow(bool bCastsShadow) { this->bCastsShadow = bCastsShadow; }
    inline bool isCastsShadow() { return bCastsShadow; }

    inline bool hasBones() { return deforms.size() > 0; }

protected:
    void rebuildTangents();
    void getTangentBitangent(VertexDataUV &v1, VertexDataUV &v2, VertexDataUV &v3, Vector3 *tangent, Vector3 *bitangent);

    VertexDataType type = VertexDataType::PositionUV;
    VertexData verticies;
    int vLength = 0;
    PolygonTriPoints *polygons = nullptr;
    int pLength = 0;
    Vector4 centroid;

    unsigned int index;
    bool bCastsShadow = true;

    std::vector<Deform *> deforms;
    Sphere boundVolume;
};
