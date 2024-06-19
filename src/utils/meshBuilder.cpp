// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "meshBuilder.h"

Mesh *MeshBuilder::createCube(float size)
{
    float halfSize = size / 2.0f;

    VertexDataUV verticies[24];

    // Top
    verticies[0] = VertexDataUV(0, halfSize, halfSize, halfSize, 0.0, 0.0, 0.0f, 0.0f, 1.0f);
    verticies[1] = VertexDataUV(1, -halfSize, halfSize, halfSize, 1.0, 0.0, 0.0f, 0.0f, 1.0f);
    verticies[2] = VertexDataUV(2, -halfSize, -halfSize, halfSize, 1.0, 1.0, 0.0f, 0.0f, 1.0f);
    verticies[3] = VertexDataUV(3, halfSize, -halfSize, halfSize, 0.0, 1.0, 0.0f, 0.0f, 1.0f);

    // Bottom
    verticies[4] = VertexDataUV(4, halfSize, halfSize, -halfSize, 0.0, 0.0, 0.0f, 0.0f, -1.0f);
    verticies[5] = VertexDataUV(5, -halfSize, halfSize, -halfSize, 1.0, 0.0, 0.0f, 0.0f, -1.0f);
    verticies[6] = VertexDataUV(6, -halfSize, -halfSize, -halfSize, 1.0, 1.0, 0.0f, 0.0f, -1.0f);
    verticies[7] = VertexDataUV(7, halfSize, -halfSize, -halfSize, 0.0, 1.0, 0.0f, 0.0f, -1.0f);

    // Right
    verticies[8] = VertexDataUV(8, halfSize, halfSize, halfSize, 0.0, 0.0, 1.0f, 0.0f, 0.0f);
    verticies[9] = VertexDataUV(9, halfSize, -halfSize, halfSize, 1.0, 0.0, 1.0f, 0.0f, 0.0f);
    verticies[10] = VertexDataUV(10, halfSize, -halfSize, -halfSize, 1.0, 1.0, 1.0f, 0.0f, 0.0f);
    verticies[11] = VertexDataUV(11, halfSize, halfSize, -halfSize, 0.0, 1.0, 1.0f, 0.0f, 0.0f);

    // Left
    verticies[12] = VertexDataUV(12, -halfSize, halfSize, halfSize, 0.0, 0.0, -1.0f, 0.0f, 0.0f);
    verticies[13] = VertexDataUV(13, -halfSize, -halfSize, halfSize, 1.0, 0.0, -1.0f, 0.0f, 0.0f);
    verticies[14] = VertexDataUV(14, -halfSize, -halfSize, -halfSize, 1.0, 1.0, -1.0f, 0.0f, 0.0f);
    verticies[15] = VertexDataUV(15, -halfSize, halfSize, -halfSize, 0.0, 1.0, -1.0f, 0.0f, 0.0f);

    // Front
    verticies[16] = VertexDataUV(16, halfSize, halfSize, halfSize, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
    verticies[17] = VertexDataUV(17, -halfSize, halfSize, halfSize, 1.0, 0.0, 0.0f, 1.0f, 0.0f);
    verticies[18] = VertexDataUV(18, -halfSize, halfSize, -halfSize, 1.0, 1.0, 0.0f, 1.0f, 0.0f);
    verticies[19] = VertexDataUV(19, halfSize, halfSize, -halfSize, 0.0, 1.0, 0.0f, 1.0f, 0.0f);

    // Back
    verticies[20] = VertexDataUV(20, halfSize, -halfSize, halfSize, 0.0, 0.0, 0.0f, -1.0f, 0.0f);
    verticies[21] = VertexDataUV(21, -halfSize, -halfSize, halfSize, 1.0, 0.0, 0.0f, -1.0f, 0.0f);
    verticies[22] = VertexDataUV(22, -halfSize, -halfSize, -halfSize, 1.0, 1.0, 0.0f, -1.0f, 0.0f);
    verticies[23] = VertexDataUV(23, halfSize, -halfSize, -halfSize, 0.0, 1.0, 0.0f, -1.0f, 0.0f);

    PolygonTriPoints polygons[12];
    for (int i = 0; i < 12; i++)
    {
        polygons[i] = PolygonTriPoints({0, 0, 0});
    }

    // top
    polygons[0] = PolygonTriPoints({0, 2, 1});
    polygons[1] = PolygonTriPoints({0, 3, 2});
    // bottom
    polygons[2] = PolygonTriPoints({4, 5, 6});
    polygons[3] = PolygonTriPoints({4, 6, 7});
    // right
    polygons[4] = PolygonTriPoints({8, 10, 9});
    polygons[5] = PolygonTriPoints({8, 11, 10});
    // left
    polygons[6] = PolygonTriPoints({12, 13, 14});
    polygons[7] = PolygonTriPoints({12, 14, 15});
    // front
    polygons[8] = PolygonTriPoints({16, 17, 18});
    polygons[9] = PolygonTriPoints({16, 18, 19});
    // back
    polygons[10] = PolygonTriPoints({20, 22, 21});
    polygons[11] = PolygonTriPoints({20, 23, 22});

    return new Mesh(VertexDataType::PositionUV, verticies, 24, polygons, 12);
}

Mesh *MeshBuilder::createCubeColored(float size, Color color[8])
{
    float halfSize = size / 2.0f;

    VertexDataColored verticies[8];
    verticies[0] = VertexDataColored(0, halfSize, halfSize, halfSize, 0, 0, 1.0f, color[0].getAsUInt());
    verticies[1] = VertexDataColored(1, -halfSize, halfSize, halfSize, 0, 0, 1.0f, color[1].getAsUInt());
    verticies[2] = VertexDataColored(2, -halfSize, -halfSize, halfSize, 0, 0, 1.0f, color[2].getAsUInt());
    verticies[3] = VertexDataColored(3, halfSize, -halfSize, halfSize, 0, 0, 1.0f, color[3].getAsUInt());
    verticies[4] = VertexDataColored(4, halfSize, halfSize, -halfSize, 0, 0, 1.0f, color[4].getAsUInt());
    verticies[5] = VertexDataColored(5, -halfSize, halfSize, -halfSize, 0, 0, 1.0f, color[5].getAsUInt());
    verticies[6] = VertexDataColored(6, -halfSize, -halfSize, -halfSize, 0, 0, 1.0f, color[6].getAsUInt());
    verticies[7] = VertexDataColored(7, halfSize, -halfSize, -halfSize, 0, 0, 1.0f, color[7].getAsUInt());

    PolygonTriPoints polygons[12];
    for (int i = 0; i < 12; i++)
    {
        polygons[i] = PolygonTriPoints({0, 0, 0});
    }
    // top
    polygons[0] = PolygonTriPoints({0, 2, 1});
    polygons[1] = PolygonTriPoints({0, 3, 2});
    // bottom
    polygons[2] = PolygonTriPoints({4, 5, 6});
    polygons[3] = PolygonTriPoints({4, 6, 7});
    // right
    polygons[4] = PolygonTriPoints({0, 4, 3});
    polygons[5] = PolygonTriPoints({3, 4, 7});
    // left
    polygons[6] = PolygonTriPoints({1, 2, 5});
    polygons[7] = PolygonTriPoints({2, 6, 5});
    // foreward
    polygons[8] = PolygonTriPoints({0, 1, 4});
    polygons[9] = PolygonTriPoints({1, 5, 4});
    // backward
    polygons[10] = PolygonTriPoints({2, 3, 6});
    polygons[11] = PolygonTriPoints({3, 7, 6});

    return new Mesh(VertexDataType::PositionColor, verticies, 8, polygons, 12);
}

Mesh *MeshBuilder::createCubeColored(float size, Color color)
{
    Color vColor[8];
    for (int i = 0; i < 8; i++)
        vColor[i] = color;
    return createCubeColored(size, vColor);
}