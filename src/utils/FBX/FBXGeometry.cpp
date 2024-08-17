// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "FBXGeometry.h"

FBXGeometry::FBXGeometry(FBXNode *node)
{
    FBXNode *vertices = node->findNode("Vertices");
    FBXNode *polygonVertexIndex = node->findNode("PolygonVertexIndex");
    FBXNode *elementUVs = node->findNode("LayerElementUV");
    FBXNode *elementNormals = node->findNode("LayerElementNormal");

    id = *static_cast<unsigned long *>(node->bindedData.at(0).data);

    if (vertices && !vertices->bindedData.empty())
    {
        auto nodeData = vertices->bindedData.at(0);
        provideVertex((double *)nodeData.data, nodeData.numElements);
    }
    if (polygonVertexIndex && !polygonVertexIndex->bindedData.empty())
    {
        auto nodeData = polygonVertexIndex->bindedData.at(0);
        providePolygonIndexes((int *)nodeData.data, nodeData.numElements);
    }
    if (elementUVs)
    {
        FBXNode *UVs = elementUVs->findNode("UV");
        FBXNode *UVIndexes = elementUVs->findNode("UVIndex");
        if (UVs && !UVs->bindedData.empty())
        {
            auto nodeData = UVs->bindedData.at(0);
            provideUVData((double *)nodeData.data, nodeData.numElements);
        }
        if (UVIndexes && !UVIndexes->bindedData.empty())
        {
            auto nodeData = UVIndexes->bindedData.at(0);
            provideUVIndexes((int *)nodeData.data, nodeData.numElements);
        }
    }
    if (elementNormals)
    {
        FBXNode *normals = elementNormals->findNode("Normals");
        if (normals && normals->bindedData.size())
        {
            auto nodeData = normals->bindedData.at(0);
            provideNormals((double *)nodeData.data, nodeData.numElements);
        }
    }
}

FBXGeometry::~FBXGeometry()
{
    if (verteces)
        delete[] verteces;
    if (normals)
        delete[] normals;
    if (uvs)
        delete[] uvs;
    if (uvIndexes)
        delete[] uvIndexes;
    if (polygonIndexes)
        delete[] polygonIndexes;
    if (outVertex)
        delete[] outVertex;
    if (outPolygons)
        delete[] outPolygons;
}

Mesh *FBXGeometry::getMesh()
{
    rebuild();
    if (!mesh)
    {
        mesh = new Mesh(VertexDataType::PositionUV, getVerteces(), getVertexAmount(), getPolygons(), getPolygonsAmount());
        if (deforms.size() > 0)
        {
            for (auto &it : deforms)
                processDeformToMesh(mesh, it);
        }
    }
    return mesh;
}

void FBXGeometry::addDeformer(FBXDeform *deform)
{
    this->deforms.push_back(deform);
}

bool FBXGeometry::hasDeformer(FBXDeform *deform)
{
    for (auto &it : deforms)
    {
        if (it == deform || it->hasDeformer(deform))
            return true;
    }
    return false;
}

void FBXGeometry::processDeformToMesh(Mesh *mesh, FBXDeform *deform)
{
    if (deform->getIndexiesAmount() == deform->getWeightsAmount())
    {
        if (deform->getIndexiesAmount() > 0)
        {
            Deform *newDeform = new Deform(deform->getName(), deform->getIndexies(), deform->getWeights(), deform->getWeightsAmount(), deform->getInvBindMatrix());
            newDeform->udpateCullingRadius(mesh);
            mesh->addDeform(newDeform);
        }
        for (auto &child : *deform->getChildren())
        {
            processDeformToMesh(mesh, child);
        }
    }
    else
        printf("Wrong amount of weights and verticies on %s\n", deform->getName().c_str());
}

void FBXGeometry::provideVertex(double *list, int countOfDoubles)
{
    if (verteces)
        delete[] verteces;

    vertexAmount = countOfDoubles / 3;
    verteces = new Vector3[vertexAmount];

    for (int i = 0; i < vertexAmount; i++)
    {
        int s = i * 3;
        verteces[i].x = static_cast<float>(list[s]);
        verteces[i].y = static_cast<float>(list[s + 1]);
        verteces[i].z = static_cast<float>(list[s + 2]);
    }

    dirty = true;
}

void FBXGeometry::providePolygonIndexes(int *list, int countOfIndexes)
{
    polygonIndexes = new int[countOfIndexes];
    memcpy(polygonIndexes, list, sizeof(int) * countOfIndexes);
    polygonIndexesAmount = countOfIndexes;

    dirty = true;
}

void FBXGeometry::provideUVData(double *list, int countOfDoubles)
{
    uvsAmount = countOfDoubles / 2;
    uvs = new Vector2[uvsAmount];

    for (int i = 0; i < uvsAmount; i++)
    {
        int shift = i * 2;
        uvs[i].x = list[shift];
        uvs[i].y = 1.0f - list[shift + 1];
    }

    dirty = true;
}

void FBXGeometry::provideUVIndexes(int *list, int countOfIndexes)
{
    uvIndexes = new int[countOfIndexes];
    memcpy(uvIndexes, list, sizeof(int) * countOfIndexes);
    uvIndexesAmount = countOfIndexes;

    dirty = true;
}

void FBXGeometry::provideNormals(double *list, int countOfDoubles)
{
    if (normals)
        delete[] normals;

    normalsAmount = countOfDoubles / 3;
    normals = new Vector3[normalsAmount];

    for (int i = 0; i < normalsAmount; i++)
    {
        int s = i * 3;
        normals[i].x = (float)list[s];
        normals[i].y = (float)list[s + 1];
        normals[i].z = (float)list[s + 2];
        normals[i] = glm::normalize(normals[i]);
    }

    dirty = true;
}

void FBXGeometry::rebuild()
{
    if (dirty && verteces && normals && uvs && uvIndexes)
    {
        dirty = false;

        std::vector<VertexDataUV> vecOutVerteces;
        std::vector<PolygonTriPoints> vecOutPolygons;

        std::vector<PolygonTriPointsUV> vecPolygons;
        PolygonTriPointsUV p;
        p.a = p.b = p.c = 0.0f;
        int filled = 0;
        for (int i = 0; i < polygonIndexesAmount; i++)
        {
            int index = polygonIndexes[i];
            int uvIndex = uvIndexes[i];

            bool reset = false;
            if (index < 0)
            {
                index = ~index;
                reset = true;
            }

            if (filled == 0)
            {
                p.a = index;
                p.na = i;
                p.UVa = uvIndex;
                filled++;
            }
            else if (filled == 1)
            {
                p.b = index;
                p.nb = i;
                p.UVb = uvIndex;
                filled++;
            }
            else if (filled == 2)
            {
                p.c = index;
                p.nc = i;
                p.UVc = uvIndex;
                filled++;
                vecPolygons.push_back(p);
            }
            else
            {
                p.b = p.a;
                p.nb = p.na;
                p.UVb = p.UVa;
                p.a = p.c;
                p.na = p.nc;
                p.UVa = p.UVc;
                p.c = index;
                p.nc = i;
                p.UVc = uvIndex;

                int sw = p.a;
                int swn = p.na;
                int UVsv = p.UVa;

                p.a = p.b;
                p.na = p.nb;
                p.UVa = p.UVb;
                p.b = sw;
                p.nb = swn;
                p.UVb = UVsv;

                vecPolygons.push_back(p);
            }

            if (reset)
                filled = 0;
        }

        unsigned int polygonsAmount = vecPolygons.size();

        for (int i = 0; i < polygonsAmount; i++)
        {
            unsigned int indexA = vecPolygons.at(i).b;
            unsigned int indexB = vecPolygons.at(i).a;
            unsigned int indexC = vecPolygons.at(i).c;

            unsigned int normalA = vecPolygons.at(i).nb;
            unsigned int normalB = vecPolygons.at(i).na;
            unsigned int normalC = vecPolygons.at(i).nc;

            unsigned int indexUVA = vecPolygons.at(i).UVb;
            unsigned int indexUVB = vecPolygons.at(i).UVa;
            unsigned int indexUVC = vecPolygons.at(i).UVc;

            VertexDataUV vecOutVertexA = VertexDataUV(indexA, verteces[indexA], uvs[indexUVA], normals[normalA]);
            VertexDataUV vecOutVertexB = VertexDataUV(indexB, verteces[indexB], uvs[indexUVB], normals[normalB]);
            VertexDataUV vecOutVertexC = VertexDataUV(indexC, verteces[indexC], uvs[indexUVC], normals[normalC]);

            PolygonTriPoints tri;
            tri.a = getIndex(vecOutVertexA, &vecOutVerteces);
            tri.b = getIndex(vecOutVertexB, &vecOutVerteces);
            tri.c = getIndex(vecOutVertexC, &vecOutVerteces);
            vecOutPolygons.push_back(tri);
        }

        if (outVertex)
            delete[] outVertex;

        if (outPolygons)
            delete[] outPolygons;

        outVertexAmount = vecOutVerteces.size();
        outVertex = new VertexDataUV[outVertexAmount];

        outPolygonsAmount = vecOutPolygons.size();
        outPolygons = new PolygonTriPoints[outPolygonsAmount];

        for (int i = 0; i < outVertexAmount; i++)
            outVertex[i] = vecOutVerteces.at(i);
        for (int i = 0; i < outPolygonsAmount; i++)
            outPolygons[i] = vecOutPolygons.at(i);
    }
}

unsigned int FBXGeometry::getIndex(const VertexDataUV &newVertex, std::vector<VertexDataUV> *vecOutVerteces)
{
    int len = vecOutVerteces->size();
    for (int index = 0; index < len; index++)
    {
        if (vecOutVerteces->at(index).position.x == newVertex.position.x &&
            vecOutVerteces->at(index).position.y == newVertex.position.y &&
            vecOutVerteces->at(index).position.z == newVertex.position.z &&
            vecOutVerteces->at(index).normal.x == newVertex.normal.x &&
            vecOutVerteces->at(index).normal.y == newVertex.normal.y &&
            vecOutVerteces->at(index).normal.z == newVertex.normal.z &&
            vecOutVerteces->at(index).uv.x == newVertex.uv.x &&
            vecOutVerteces->at(index).uv.y == newVertex.uv.y &&
            vecOutVerteces->at(index).index == newVertex.index)
        {
            return index;
        }
    }
    vecOutVerteces->push_back(newVertex);
    return vecOutVerteces->size() - 1;
}
