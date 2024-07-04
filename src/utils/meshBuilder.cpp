// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshBuilder.h"

Mesh *MeshBuilder::createCube(float size)
{
    float halfSize = size / 2.0f;

    VertexDataUV verticies[24];

    // Front
    verticies[0] = VertexDataUV(0, halfSize, halfSize, halfSize, 1.0, 0.0, 0.0f, 0.0f, 1.0f);
    verticies[1] = VertexDataUV(1, -halfSize, halfSize, halfSize, 0.0, 0.0, 0.0f, 0.0f, 1.0f);
    verticies[2] = VertexDataUV(2, -halfSize, -halfSize, halfSize, 0.0, 1.0, 0.0f, 0.0f, 1.0f);
    verticies[3] = VertexDataUV(3, halfSize, -halfSize, halfSize, 1.0, 1.0, 0.0f, 0.0f, 1.0f);

    // Back
    verticies[4] = VertexDataUV(4, -halfSize, -halfSize, -halfSize, 0.0, 0.0, 0.0f, 0.0f, -1.0f);
    verticies[5] = VertexDataUV(5, halfSize, -halfSize, -halfSize, 1.0, 0.0, 0.0f, 0.0f, -1.0f);
    verticies[6] = VertexDataUV(6, halfSize, halfSize, -halfSize, 1.0, 1.0, 0.0f, 0.0f, -1.0f);
    verticies[7] = VertexDataUV(7, -halfSize, halfSize, -halfSize, 0.0, 1.0, 0.0f, 0.0f, -1.0f);

    // Right
    verticies[8] = VertexDataUV(8, halfSize, halfSize, halfSize, 1.0, 1.0, 1.0f, 0.0f, 0.0f);
    verticies[9] = VertexDataUV(9, halfSize, -halfSize, halfSize, 1.0, 0.0, 1.0f, 0.0f, 0.0f);
    verticies[10] = VertexDataUV(10, halfSize, -halfSize, -halfSize, 0.0, 0.0, 1.0f, 0.0f, 0.0f);
    verticies[11] = VertexDataUV(11, halfSize, halfSize, -halfSize, 0.0, 1.0, 1.0f, 0.0f, 0.0f);

    // Left
    verticies[12] = VertexDataUV(12, -halfSize, -halfSize, -halfSize, 1.0, 0.0, -1.0f, 0.0f, 0.0f);
    verticies[13] = VertexDataUV(13, -halfSize, halfSize, -halfSize, 1.0, 1.0, -1.0f, 0.0f, 0.0f);
    verticies[14] = VertexDataUV(14, -halfSize, halfSize, halfSize, 0.0, 1.0, -1.0f, 0.0f, 0.0f);
    verticies[15] = VertexDataUV(15, -halfSize, -halfSize, halfSize, 0.0, 0.0, -1.0f, 0.0f, 0.0f);

    // Top
    verticies[16] = VertexDataUV(16, -halfSize, halfSize, -halfSize, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
    verticies[17] = VertexDataUV(17, halfSize, halfSize, -halfSize, 1.0, 0.0, 0.0f, 1.0f, 0.0f);
    verticies[18] = VertexDataUV(18, halfSize, halfSize, halfSize, 1.0, 1.0, 0.0f, 1.0f, 0.0f);
    verticies[19] = VertexDataUV(19, -halfSize, halfSize, halfSize, 0.0, 1.0, 0.0f, 1.0f, 0.0f);

    // Bottom
    verticies[20] = VertexDataUV(20, halfSize, -halfSize, halfSize, 0.0, 0.0, 0.0f, -1.0f, 0.0f);
    verticies[21] = VertexDataUV(21, -halfSize, -halfSize, halfSize, 0.0, 1.0, 0.0f, -1.0f, 0.0f);
    verticies[22] = VertexDataUV(22, -halfSize, -halfSize, -halfSize, 1.0, 1.0, 0.0f, -1.0f, 0.0f);
    verticies[23] = VertexDataUV(23, halfSize, -halfSize, -halfSize, 1.0, 0.0, 0.0f, -1.0f, 0.0f);

    PolygonTriPoints polygons[12];
    for (int i = 0; i < 12; i++)
    {
        polygons[i] = PolygonTriPoints({0, 0, 0});
    }

    // Front
    polygons[0] = PolygonTriPoints({0, 2, 1});
    polygons[1] = PolygonTriPoints({0, 3, 2});
    // Back
    polygons[2] = PolygonTriPoints({4, 5, 6});
    polygons[3] = PolygonTriPoints({4, 6, 7});
    // right
    polygons[4] = PolygonTriPoints({8, 10, 9});
    polygons[5] = PolygonTriPoints({8, 11, 10});
    // left
    polygons[6] = PolygonTriPoints({12, 13, 14});
    polygons[7] = PolygonTriPoints({12, 14, 15});
    // top
    polygons[8] = PolygonTriPoints({17, 18, 16});
    polygons[9] = PolygonTriPoints({16, 18, 19});
    // bottom
    polygons[10] = PolygonTriPoints({20, 22, 21});
    polygons[11] = PolygonTriPoints({20, 23, 22});

    return new Mesh(VertexDataType::PositionUV, verticies, 24, polygons, 12);
}

Mesh *MeshBuilder::createSphere(float radius, unsigned int rings, unsigned int segments)
{
    std::vector<VertexDataUV> verticies;
    std::vector<PolygonTriPoints> polygons;

    int index = 0;
    for (unsigned int y = 0; y <= rings; ++y)
    {
        for (unsigned int x = 0; x <= segments; ++x)
        {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)rings;
            float xPos = radius * cos(xSegment * 2.0f * CONST_PI) * sin(ySegment * CONST_PI);
            float yPos = radius * cos(ySegment * CONST_PI);
            float zPos = radius * sin(xSegment * 2.0f * CONST_PI) * sin(ySegment * CONST_PI);

            Vector3 normal = glm::normalize(Vector3(xPos, yPos, zPos));
            verticies.push_back({index, xPos, yPos, zPos, xSegment, ySegment, normal.x, normal.y, normal.z});
            index++;
        }
    }

    for (unsigned int y = 0; y < rings; ++y)
    {
        for (unsigned int x = 0; x < segments; ++x)
        {
            unsigned int first = (y * (segments + 1)) + x;
            unsigned int second = first + segments + 1;

            polygons.push_back({first, second, first + 1});
            polygons.push_back({second, second + 1, first + 1});
        }
    }

    return new Mesh(VertexDataType::PositionUV, &verticies.front(), verticies.size(), &polygons.front(), polygons.size());
}