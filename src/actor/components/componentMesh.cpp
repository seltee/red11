// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentMesh.h"

ComponentMesh::ComponentMesh()
{
}

void ComponentMesh::setMesh(Mesh *mesh)
{
    this->mesh = mesh;
}

void ComponentMesh::setMaterial(Material *material)
{
    this->material = material;
}

void ComponentMesh::onRenderQueue(Renderer *renderer)
{
    if (mesh)
    {
        renderer->queueMesh(mesh, material, getModelMatrix());

        if (bShowDebugNormals && getModelMatrix())
        {
            Matrix3 mModelInverseTranspose;
            Matrix4 *transformation = getModelMatrix();

            if (transformation)
                mModelInverseTranspose = (Matrix3)glm::transpose(glm::inverse(*transformation));

            int vAmount = mesh->getVerticiesAmount();
            auto verticies = mesh->getVerticies();
            for (int i = 0; i < vAmount; i++)
            {
                Vector3 p = (Vector3)(*transformation * Vector4(verticies->vertexPositionUV[i].position, 1.0f));
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