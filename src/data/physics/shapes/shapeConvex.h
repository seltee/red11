// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "utils/utils.h"
#include "utils/math.h"
#include "data/mesh.h"

class PhysicsWorld;
class Mesh;

class ShapeConvex : public Shape
{
public:
    EXPORT ShapeConvex(float simScale, Vector3 *verticies, int verticiesAmount, HullPolygon *polygons, int polygonsAmount, float density);
    EXPORT ShapeConvex(float simScale, Mesh *mesh, int limitToNumber, float density);

    EXPORT void recalcCenter();
    EXPORT void recalcAABB();

    EXPORT ShapeCollisionType getType() override;
    EXPORT AABB getAABB(const Matrix4 &model) override final;

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) override final;

    inline Vector3 *getVerticies() { return verticies; }
    inline int getVerticiesAmount() { return verticiesAmount; }

    inline HullPolygon *getPolygons() { return polygons; }
    inline int getPolygonsAmount() { return polygonsAmount; }

    inline HullEdge *getEdges() { return hullEdges; };
    inline int getEdgesAmount() { return hullEdgesAmount; };

protected:
    void buildHull(Mesh *mesh, int limitToNumber, float simScale);

    Vector3 center;
    Vector3 *verticies;
    int verticiesAmount;
    HullPolygon *polygons;
    int polygonsAmount;

    HullEdge *hullEdges;
    int hullEdgesAmount;

    float aabbRadius = 0.0f;
};
