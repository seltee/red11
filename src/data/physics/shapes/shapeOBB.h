// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "shapeConvex.h"
#include "utils/utils.h"
#include "utils/math.h"

class PhysicsWorld;

class ShapeOBB : public Shape
{
public:
    EXPORT ShapeOBB(const Vector3 &center, float width, float height, float depth, float density);

    EXPORT ShapeCollisionType getType() override final;
    EXPORT AABB getAABB(const Matrix4 &model) override final;

    EXPORT int castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache) override final;

    EXPORT Vector3 getClosestPoint(const Matrix4 &OBBTransformation, Vector3 point);

    inline HullPolygon *getPolygons() { return hullPolygons; };
    inline HullEdge *getEdges() { return hullEdges; };
    inline int getEdgesAmount() { return hullEdgesAmount; };

    inline float getWidth() { return width; }
    inline float getHeight() { return height; }
    inline float getDepth() { return depth; }
    inline float getHalfWidth() { return halfWidth; }
    inline float getHalfHeight() { return halfHeight; }
    inline float getHalfDepth() { return halfDepth; }

protected:
    void rebuildPolygons();
    Vector3 getRayPointNormal(float tDistance, Vector3 &tMin, Vector3 &tMax, const Vector3 &invDir, Matrix4 &transformation);

    float width, height, depth;
    float halfWidth, halfHeight, halfDepth;
    Vector3 center;

    HullPolygon *hullPolygons;
    HullEdge *hullEdges;
    int hullEdgesAmount;

    static HullPolygon hullPolygonsStatic[6];
    static HullEdge *hullEdgesStatic;
    static int hullEdgesAmountStatic;
};
