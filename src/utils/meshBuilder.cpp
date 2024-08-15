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

Mesh *MeshBuilder::createPlain(float size)
{
    return createPlain(size, size);
}

Mesh *MeshBuilder::createPlain(float width, float height)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    VertexDataUV verticies[4];
    verticies[0] = VertexDataUV(0, -halfWidth, 0.0f, -halfHeight, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
    verticies[1] = VertexDataUV(1, halfWidth, 0.0f, -halfHeight, 1.0, 0.0, 0.0f, 1.0f, 0.0f);
    verticies[2] = VertexDataUV(2, halfWidth, 0.0f, halfHeight, 1.0, 1.0, 0.0f, 1.0f, 0.0f);
    verticies[3] = VertexDataUV(3, -halfWidth, 0.0f, halfHeight, 0.0, 1.0, 0.0f, 1.0f, 0.0f);

    PolygonTriPoints polygons[2];
    polygons[0] = PolygonTriPoints({1, 2, 0});
    polygons[1] = PolygonTriPoints({0, 2, 3});

    return new Mesh(VertexDataType::PositionUV, verticies, 4, polygons, 2);
}

Mesh *MeshBuilder::createSprite(float size)
{
    VertexDataUV verticies[4];
    verticies[0] = VertexDataUV(0, 0.0f, 0.0f, 0.0f, 0.0, 0.0, 0.0f, 0.0f, 1.0f);
    verticies[1] = VertexDataUV(1, size, 0.0f, 0.0f, 1.0, 0.0, 0.0f, 0.0f, 1.0f);
    verticies[2] = VertexDataUV(2, size, size, 0.0f, 1.0, 1.0, 0.0f, 0.0f, 1.0f);
    verticies[3] = VertexDataUV(3, 0.0f, size, 0.0f, 0.0, 1.0, 0.0f, 0.0f, 1.0f);

    PolygonTriPoints polygons[2];
    polygons[0] = PolygonTriPoints({1, 2, 0});
    polygons[1] = PolygonTriPoints({0, 2, 3});

    return new Mesh(VertexDataType::PositionUV, verticies, 4, polygons, 2);
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

Mesh *MeshBuilder::createCapsule(const Vector3 &pa, const Vector3 &pb, float radius, unsigned int segments)
{
    std::vector<VertexDataUV> verticies;
    std::vector<PolygonTriPoints> polygons;

    Vector3 up = glm::length(pb - pa) == 0.0f ? Vector3(0.0f, 1.0f, 0.0f) : glm::normalize(pb - pa);
    Vector3 right = (up == Vector3(0.0f, 1.0f, 0.0f) || up == Vector3(0.0f, -1.0f, 0.0f)) ? Vector3(1.0f, 0.0f, 0.0f) : glm::normalize(glm::cross(up, Vector3(0.0f, 1.0f, 0.0f)));
    Vector3 forward = glm::normalize(glm::cross(right, up));

    Matrix4 transform = Matrix4(1.0f);
    transform[0] = Vector4(right, 0.0f);
    transform[1] = Vector4(up, 0.0f);
    transform[2] = Vector4(forward, 0.0f);

    // Generate vertices and indices for the cylinder part
    for (int i = 0; i < segments; ++i)
    {
        float theta = CONST_PI2 * static_cast<float>(i) / static_cast<float>(segments);
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        Vector3 offset = radius * (cosTheta * right + sinTheta * forward);

        Vector3 posA = pa + offset;
        Vector3 posB = pb + offset;
        Vector3 normal = glm::normalize(offset);
        Vector2 uv = Vector2(static_cast<float>(i) / segments, 0.0f);

        int cv = verticies.size();
        verticies.push_back({cv, posA.x, posA.y, posA.z, uv.x, uv.y, normal.x, normal.y, normal.z});
        verticies.push_back({cv + 1, posB.x, posB.y, posB.z, uv.x, uv.y, normal.x, normal.y, normal.z});

        unsigned int a = cv;
        unsigned int b = cv + 1;
        unsigned int c = i < segments - 1 ? cv + 3 : 1;
        unsigned int d = i < segments - 1 ? cv + 2 : 0;

        polygons.push_back({a, c, b});
        polygons.push_back({a, d, c});
    }

    // Function to generate a hemisphere
    auto generateHemisphere = [&](const Vector3 &center, const Vector3 &direction, int baseVNum)
    {
        Vector3 localUp = direction;
        Vector3 localRight = (direction == Vector3(0.0f, 1.0f, 0.0f) || direction == Vector3(0.0f, -1.0f, 0.0f)) ? Vector3(1.0f, 0.0f, 0.0f) : glm::normalize(glm::cross(localUp, Vector3(0.0f, 1.0f, 0.0f)));
        Vector3 localForward = glm::normalize(glm::cross(localRight, localUp));

        int capSegments = (segments / 2 > 2) ? segments / 2 : 2;
        for (int j = 1; j <= capSegments; j++)
        {
            float theta = CONST_PI * 0.5f - CONST_PI * 0.5f * static_cast<float>(j) / static_cast<float>(capSegments);
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            if (j < capSegments)
            {
                for (int i = 0; i < segments; i++)
                {
                    float phi = CONST_PI2 * static_cast<float>(i) / static_cast<float>(segments) * (baseVNum == 0 ? -1.0f : 1.0f);
                    float sinPhi = sin(phi);
                    float cosPhi = cos(phi);

                    Vector3 localPos = radius * (sinTheta * cosPhi * localRight + sinTheta * sinPhi * localForward + cosTheta * localUp);
                    Vector3 pos = center + localPos;
                    Vector3 normal = glm::normalize(localPos);
                    Vector2 uv = Vector2(static_cast<float>(i) / static_cast<float>(segments), static_cast<float>(j) / static_cast<float>(segments));

                    int cvTop = verticies.size();
                    int cvBottom = j == 1 ? baseVNum + i * 2 : cvTop - segments;
                    int bottomStep = j == 1 ? 2 : 1;

                    verticies.push_back({cvTop, pos.x, pos.y, pos.z, uv.x, uv.y, normal.x, normal.y, normal.z});

                    unsigned int a = cvTop;
                    unsigned int b = cvBottom;
                    unsigned int c = i < segments - 1 ? cvBottom + bottomStep : cvBottom - segments * bottomStep + bottomStep;
                    unsigned int d = i < segments - 1 ? cvTop + 1 : cvTop - segments + 1;

                    if (baseVNum == 0)
                    {
                        polygons.push_back({a, c, b});
                        polygons.push_back({a, d, c});
                    }
                    else
                    {
                        polygons.push_back({a, b, c});
                        polygons.push_back({a, c, d});
                    }
                }
            }
            else
            {
                Vector3 localPos = radius * localUp;
                Vector3 pos = center + localPos;
                Vector3 normal = glm::normalize(localPos);
                Vector2 uv = Vector2(0, 0);
                int cvTop = verticies.size();
                verticies.push_back({cvTop, pos.x, pos.y, pos.z, uv.x, uv.y, normal.x, normal.y, normal.z});

                for (int i = 0; i < segments; i++)
                {
                    int cvBottom = cvTop - segments + i;
                    unsigned int a = cvTop;
                    unsigned int b = cvBottom;
                    unsigned int c = i < segments - 1 ? cvBottom + 1 : cvBottom - segments + 1;

                    if (baseVNum == 0)
                        polygons.push_back({a, c, b});
                    else
                        polygons.push_back({a, b, c});
                }
            }
        }
    };

    // Generate top hemisphere
    generateHemisphere(pb, up, 1);
    // Generate bottom hemisphere
    generateHemisphere(pa, -up, 0);

    return new Mesh(VertexDataType::PositionUV, &verticies.front(), verticies.size(), &polygons.front(), polygons.size());
}