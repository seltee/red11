// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshCombiner.h"

Mesh *combineMeshList(std::vector<MeshObject *> &list)
{
    std::vector<Mesh *> transformedList;
    if (list.size() == 0)
        return nullptr;

    // check if all data types are the same
    VertexDataType type = list.at(0)->getMesh()->getType();
    for (auto &it : list)
        if (it->getMesh()->getType() != type)
            return nullptr;

    for (auto &it : list)
        transformedList.push_back(it->getTransformedMesh());

    // collect amount of data and create arrays
    int totalPolygonsAmount = 0;
    int totalVerticiesAmount = 0;

    for (auto &it : transformedList)
    {
        totalPolygonsAmount += it->getPolygonsAmount();
        totalVerticiesAmount += it->getVerticiesAmount();
    }

    Mesh *outMesh = nullptr;
    if (totalPolygonsAmount && totalVerticiesAmount)
    {
        PolygonTriPoints *polygons = new PolygonTriPoints[totalPolygonsAmount];
        VertexData verticies;
        if (type == VertexDataType::PositionUV)
            verticies.vertexPositionUV = new VertexDataUV[totalVerticiesAmount];
        if (type == VertexDataType::PositionColor)
            verticies.vertexPositionColor = new VertexDataColored[totalVerticiesAmount];

        int vertShift = 0;
        int polShift = 0;
        for (auto &it : transformedList)
        {
            if (type == VertexDataType::PositionUV)
                memcpy(&verticies.vertexPositionUV[vertShift], it->getVerticies()->ptr, sizeof(VertexDataUV) * it->getVerticiesAmount());
            if (type == VertexDataType::PositionColor)
                memcpy(&verticies.vertexPositionColor[vertShift], it->getVerticies()->ptr, sizeof(VertexDataColored) * it->getVerticiesAmount());
            memcpy(&polygons[polShift], it->getPolygons(), sizeof(PolygonTriPoints) * it->getPolygonsAmount());

            for (int i = 0; i < it->getPolygonsAmount(); i++)
            {
                polygons[polShift + i].a += vertShift;
                polygons[polShift + i].b += vertShift;
                polygons[polShift + i].c += vertShift;
            }

            vertShift += it->getVerticiesAmount();
            polShift += it->getPolygonsAmount();
        }

        outMesh = new Mesh(type, verticies.ptr, totalVerticiesAmount, polygons, totalPolygonsAmount);

        delete[] polygons;
        if (type == VertexDataType::PositionUV)
            delete[] verticies.vertexPositionUV;
        if (type == VertexDataType::PositionColor)
            delete[] verticies.vertexPositionColor;
    }

    for (auto &it : transformedList)
        delete it;

    return outMesh;
}