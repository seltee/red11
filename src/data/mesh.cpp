// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "mesh.h"
#include "utils/makeSmallestSphere.h"
#include "renderer/renderer.h"

#define FLOAT_PREC_DIV_CONST 0.00001f

std::vector<Mesh *> Mesh::meshes;
bool Mesh::indexPool[MAX_ELEMENT_INDEX];
unsigned int Mesh::nextIndex = 0;

Mesh::Mesh(VertexDataType type, void *verticies, int vLength, PolygonTriPoints *polygons, int pLength, Matrix4 *transformation)
{
    this->type = type;
    this->vLength = vLength;
    index = getNextIndex();

    Matrix3 mModelInverseTranspose;
    if (transformation)
        mModelInverseTranspose = glm::transpose(glm::inverse(*transformation));

    Vector3 centroidCalc = Vector3(0.0f);

    if (type == VertexDataType::PositionUV)
    {
        this->verticies.vertexPositionUV = new VertexDataUV[vLength];
        for (int i = 0; i < vLength; i++)
        {
            if (transformation)
            {
                this->verticies.vertexPositionUV[i] = ((VertexDataUV *)verticies)[i];
                this->verticies.vertexPositionUV[i].position = Vector3(*transformation * Vector4(this->verticies.vertexPositionUV[i].position, 1.0f));
                this->verticies.vertexPositionUV[i].normal = glm::normalize(mModelInverseTranspose * this->verticies.vertexPositionUV[i].normal);
            }
            else
                this->verticies.vertexPositionUV[i] = ((VertexDataUV *)verticies)[i];
            centroidCalc += this->verticies.vertexPositionUV[i].position * FLOAT_PREC_DIV_CONST;
        }
    }
    if (type == VertexDataType::PositionColor)
    {
        this->verticies.vertexPositionColor = new VertexDataColored[vLength];
        for (int i = 0; i < vLength; i++)
        {
            if (transformation)
            {
                this->verticies.vertexPositionColor[i] = ((VertexDataColored *)verticies)[i];
                this->verticies.vertexPositionColor[i].position = Vector3(*transformation * Vector4(this->verticies.vertexPositionColor[i].position, 1.0f));
                this->verticies.vertexPositionColor[i].normal = glm::normalize(mModelInverseTranspose * this->verticies.vertexPositionUV[i].normal);
            }
            else
                this->verticies.vertexPositionColor[i] = ((VertexDataColored *)verticies)[i];
            centroidCalc += this->verticies.vertexPositionUV[i].position * FLOAT_PREC_DIV_CONST;
        }
    }

    centroidCalc /= (float)vLength;
    centroidCalc /= FLOAT_PREC_DIV_CONST;
    centroid = Vector4(centroidCalc, 1.0f);

    this->polygons = new PolygonTriPoints[pLength];
    this->pLength = pLength;
    for (int i = 0; i < pLength; i++)
        this->polygons[i] = polygons[i];

    boundVolume = makeSmallestSphere((const float *)this->verticies.ptr, vLength, getVertexDataTypeSize(type) / sizeof(float));

    rebuildTangents();

    meshes.push_back(this);
}

Mesh::~Mesh()
{
    unload();

    if (type == VertexDataType::PositionUV && this->verticies.vertexPositionUV)
        delete this->verticies.vertexPositionUV;
    if (type == VertexDataType::PositionColor && this->verticies.vertexPositionColor)
        delete this->verticies.vertexPositionColor;
    if (this->polygons)
        delete[] this->polygons;

    auto it = meshes.begin();
    while (it != meshes.end())
    {
        if ((*it) == this)
        {
            meshes.erase(it);
            break;
        }
        it++;
    }

    indexPool[index] = false;
}

void Mesh::addDeform(Deform *deform)
{
    // Blender doesn't export deforms that doesn't affect mesh
    /*
    bool bAffects = false;
    for (int v = 0; v < vLength; v++)
    {
        if (deform->hasIndex(verticies.vertexPositionUV[v].index))
        {
            bAffects = true;
            break;
        }
    }
    if (!bAffects)
        return;
    */

    // TODO calc visibility of bone animated meshes by their bone volumes
    if (deforms.size() == 0)
        boundVolume.radius *= 1.25f;

    deform->index = deforms.size();
    deforms.push_back(deform);
}

bool Mesh::isLoaded()
{
    return true;
}

void Mesh::load()
{
}

void Mesh::unload()
{
    Renderer::removeFromAllMeshByIndex(index);
}

void Mesh::rebuildTangents()
{
    for (int i = 0; i < vLength; i++)
    {
        verticies.vertexPositionUV[i].tangent = Vector3(0.0f);
        verticies.vertexPositionUV[i].bitangent = Vector3(0.0f);
    }

    Vector3 tangent, bitangent;
    for (int i = 0; i < pLength; i++)
    {
        PolygonTriPoints p = polygons[i];
        getTangentBitangent(verticies.vertexPositionUV[p.a], verticies.vertexPositionUV[p.b], verticies.vertexPositionUV[p.c], &tangent, &bitangent);
        verticies.vertexPositionUV[p.a].tangent += tangent;
        verticies.vertexPositionUV[p.b].tangent += tangent;
        verticies.vertexPositionUV[p.c].tangent += tangent;
        verticies.vertexPositionUV[p.a].bitangent += bitangent;
        verticies.vertexPositionUV[p.b].bitangent += bitangent;
        verticies.vertexPositionUV[p.c].bitangent += bitangent;
    }

    for (int i = 0; i < vLength; i++)
    {
        verticies.vertexPositionUV[i].tangent = glm::normalize(verticies.vertexPositionUV[i].tangent);
        verticies.vertexPositionUV[i].bitangent = glm::normalize(verticies.vertexPositionUV[i].bitangent);

        // Ensure right-handed coordinate system
        if (glm::dot(glm::cross(verticies.vertexPositionUV[i].normal, verticies.vertexPositionUV[i].tangent), verticies.vertexPositionUV[i].bitangent) < 0.0f)
        {
            verticies.vertexPositionUV[i].tangent = -verticies.vertexPositionUV[i].tangent;
        }
    }
}

void Mesh::getTangentBitangent(VertexDataUV &v1, VertexDataUV &v2, VertexDataUV &v3, Vector3 *tangent, Vector3 *bitangent)
{
    // Edges of the triangle
    Vector3 edge1 = v2.position - v1.position;
    Vector3 edge2 = v3.position - v1.position;
    Vector2 deltaUV1 = v2.uv - v1.uv;
    Vector2 deltaUV2 = v3.uv - v1.uv;

    // Calculate the tangent and bitangent
    float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
    if (fabs(f) < 1e-6)
    {
        *tangent = Vector3(1.0f, 0.0f, 0.0f);
        *bitangent = Vector3(0.0f, 1.0f, 0.0f);
        return;
    }
    f = 1.0f / f;

    *tangent = glm::normalize(Vector3(
        f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
        f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
        f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)));

    *bitangent = glm::normalize(Vector3(
        f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
        f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
        f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)));
}

unsigned int Mesh::getNextIndex()
{
    while (indexPool[nextIndex])
    {
        nextIndex++;
        if (nextIndex >= MAX_ELEMENT_INDEX)
            nextIndex = 0;
    }

    indexPool[nextIndex] = true;
    return nextIndex;
}