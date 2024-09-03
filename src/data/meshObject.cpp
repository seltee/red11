// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshObject.h"

MeshObject::MeshObject(Mesh *mesh, bool bIsBone)
{
    this->mesh = mesh;
    this->bIsBone = bIsBone;

    static unsigned int lastIndex = 0;
    index = lastIndex;
    lastIndex++;
}

void MeshObject::setParent(MeshObject *parent)
{
    this->parent = parent;
    setTransformationParent(parent);
}

void MeshObject::setEntityParent(Entity *entityParent)
{
    this->parent = nullptr;
    setTransformationParent(entityParent);
}

Mesh *MeshObject::getTransformedMesh()
{
    if (mesh)
    {
        return new Mesh(mesh->getType(), mesh->getVerticies()->ptr, mesh->getVerticiesAmount(), mesh->getPolygons(), mesh->getPolygonsAmount(), &getModelMatrix());
    }
    return nullptr;
}

Mesh *MeshObject::getMesh()
{
    return mesh;
}

MeshObject *MeshObject::clone()
{
    MeshObject *obj = new MeshObject(mesh);
    obj->setPosition(getPosition());
    obj->setRotation(getRotation());
    obj->setScale(getScale());
    obj->setParent(getParent());
    return obj;
}

void MeshObject::setVertices(const Vector3 *list, int amount, const Matrix4 &transformation)
{
    if (this->vertices)
        delete[] this->vertices;
    this->vertices = nullptr;

    if (amount)
    {
        this->vertices = new Vector3[amount];
        this->verticesAmount = amount;

        for (int i = 0; i < amount; i++)
            this->vertices[i] = Vector3(transformation * Vector4(list[i], 1.0f));
    }
}

void MeshObject::setVertices(const VertexDataUV *list, int amount, const Matrix4 &transformation)
{
    if (this->vertices)
        delete[] this->vertices;
    this->vertices = nullptr;

    if (amount)
    {
        this->vertices = new Vector3[amount];
        this->verticesAmount = amount;

        for (int i = 0; i < amount; i++)
            this->vertices[i] = Vector3(transformation * Vector4(list[i].position, 1.0f));
    }
}

MeshObject::~MeshObject()
{
    if (this->vertices)
        delete[] this->vertices;
}