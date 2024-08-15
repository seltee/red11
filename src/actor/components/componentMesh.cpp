// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentMesh.h"

ComponentMesh::ComponentMesh()
{
}

ComponentMesh::~ComponentMesh()
{
    if (this->material)
        this->material->removeUser();
    if (this->mesh)
        this->mesh->removeUser();
}

void ComponentMesh::setMesh(Mesh *mesh)
{
    if (this->mesh)
        this->mesh->addUser();

    this->mesh = mesh;
    if (this->mesh)
        this->mesh->removeUser();
}

void ComponentMesh::setMaterial(Material *material)
{
    if (this->material)
        this->material->removeUser();

    this->material = material;
    if (this->material)
        this->material->addUser();
}

void ComponentMesh::onRenderQueue(Renderer *renderer)
{
    if (mesh && bVisible)
    {
        renderer->queueMesh(mesh, material, getModelMatrix());

        if (bShowDebugNormals)
        {
            const Matrix4 &transformation = getModelMatrix();
            Matrix3 mModelInverseTranspose = (Matrix3)glm::transpose(glm::inverse(transformation));

            int vAmount = mesh->getVerticiesAmount();
            auto verticies = mesh->getVerticies();
            for (int i = 0; i < vAmount; i++)
            {
                Vector3 p = (Vector3)(transformation * Vector4(verticies->vertexPositionUV[i].position, 1.0f));
                Vector3 n = glm::normalize(mModelInverseTranspose * verticies->vertexPositionUV[i].normal);
                Vector3 tg = glm::normalize(mModelInverseTranspose * verticies->vertexPositionUV[i].tangent);
                Vector3 btg = glm::normalize(mModelInverseTranspose * verticies->vertexPositionUV[i].bitangent);

                renderer->queueLine(p, p + n * 0.06f, Color(0, 0, 1.0f));
                renderer->queueLine(p, p + tg * 0.06f, Color(1.0f, 0, 0));
                renderer->queueLine(p, p + btg * 0.06f, Color(0, 1.0f, 0));
            }
        }
    }
}