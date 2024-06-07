// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshObject.h"

MeshObject::MeshObject(Mesh *mesh)
{
    this->mesh = mesh;
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
    return new Mesh(mesh->getType(), mesh->getVerticies()->ptr, mesh->getVerticiesAmount(), mesh->getPolygons(), mesh->getPolygonsAmount(), getModelMatrix());
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