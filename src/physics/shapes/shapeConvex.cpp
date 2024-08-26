#include "shapeConvex.h"
#include "data/mesh.h"
#include "utils/algorithms.h"
#include "utils/convex/convhull_3d.h"
#include <string>
#include <vector>
#include "red11.h"

struct QuickHullFace
{
    int vertices[3];
};

ShapeConvex::ShapeConvex(float simScale, Vector3 *verticies, int verticiesAmount, HullPolygon *polygons, int polygonsAmount, float density)
{
    this->verticies = new Vector3[verticiesAmount];
    this->verticiesAmount = verticiesAmount;
    for (int i = 0; i < verticiesAmount; i++)
    {
        this->verticies[i] = verticies[i] * simScale;
    }

    mass = getConvexHullVolume(this->verticies, verticiesAmount) * density;
    inertia = getConvexHullInertia(this->verticies, verticiesAmount, density);

    this->polygons = new HullPolygon[polygonsAmount];
    this->polygonsAmount = polygonsAmount;
    memcpy(this->polygons, polygons, sizeof(HullPolygon) * polygonsAmount);

    hullEdgesAmount = rebuildEdges(this->polygons, polygonsAmount, &hullEdges);

    recalcAABB();
    recalcCenter();

    /*
        BOX
        0.250000
        0.002604 0.000000 0.000000
        0.000000 0.002604 0.000000
        0.000000 0.000000 0.002604

        CONVEX
        0.114583
        0.002014 nan nan
        nan 0.002238 nan
        nan nan 0.001790
    */
    /*
        mass = 0.25f;
        inertia[0] = Vector3(0.002604, 0.000000, 0.000000);
        inertia[1] = Vector3(0.000000, 0.002604, 0.000000);
        inertia[2] = Vector3(0.000000, 0.000000, 0.002604);
    */
    /*
        printf("CONVEX\n");
        printf("%f\n", mass);
        printf("%f %f %f\n", inertia[0][0], inertia[0][1], inertia[0][2]);
        printf("%f %f %f\n", inertia[1][0], inertia[1][1], inertia[1][2]);
        printf("%f %f %f\n", inertia[2][0], inertia[2][1], inertia[2][2]);
    */
}
ShapeConvex::ShapeConvex(float simScale, Mesh *mesh, int limitToNumber, float density)
{
    buildHull(mesh, limitToNumber, simScale);
    if (this->verticiesAmount && this->polygonsAmount)
    {
        mass = getConvexHullVolume(this->verticies, this->verticiesAmount) * density;
        inertia = getConvexHullInertia(this->verticies, this->verticiesAmount, density);

        hullEdgesAmount = rebuildEdges(this->polygons, this->polygonsAmount, &hullEdges);

        recalcAABB();
        recalcCenter();
    }
}

void ShapeConvex::recalcCenter()
{
    float add = 1.0f / static_cast<float>(this->verticiesAmount);
    this->center = Vector3(0.0f);
    for (int i = 0; i < this->verticiesAmount; i++)
        this->center += this->verticies[i] * add;
}

void ShapeConvex::recalcAABB()
{
    aabbRadius = 0.0f;
    for (int i = 0; i < verticiesAmount; i++)
    {
        float dist = glm::length2(this->verticies[i]);
        if (dist > aabbRadius)
            aabbRadius = dist;
    }
    aabbRadius = sqrtf(aabbRadius) * 1.4f;
}

ShapeCollisionType ShapeConvex::getType()
{
    return ShapeCollisionType::Convex;
}

AABB ShapeConvex::getAABB(const Matrix4 &model)
{
    Vector3 modelCenter = Vector3(model * Vector4(center, 1.0f));
    return AABB(modelCenter - Vector3(aabbRadius, aabbRadius, aabbRadius), modelCenter + Vector3(aabbRadius, aabbRadius, aabbRadius));
}

int ShapeConvex::castRay(const Segment &ray, PhysicsBodyPoint *newPoints, PhysicsBodyCache *cache)
{
    // Compute direction vector for the segment
    Vector3 direction = ray.b - ray.a;

    // Set initial interval to being the whole segment.
    float pointfirst = 0.0f;
    float pointlast = 1.0f;

    // Out Normals
    Vector3 normalFirst = Vector3(0.0f);
    Vector3 normalLast = Vector3(0.0f);

    // Intersect segment against each polygon
    for (int p = 0; p < polygonsAmount; p++)
    {
        // Normal of the polygon = normal of it's plain
        Vector3 normal = cache->convex.normals[p];
        float denom = glm::dot(normal, direction);

        // Distance from center to polygon plane
        float plainDistance = glm::dot(normal, cache->convex.verticies[polygons[p].points[0]]);
        float lineOriginDistance = plainDistance - glm::dot(normal, ray.a);

        // Test if segment runs parallel to the plane
        if (denom == 0.0f)
        {
            if (lineOriginDistance > 0.0f)
                return 0;
        }
        else
        {
            // Compute parameterized t value for intersection with current plane
            float t = lineOriginDistance / denom;
            if (denom < 0.0f)
            {
                // Entering halfspace
                if (t > pointfirst)
                {
                    normalFirst = normal;
                    pointfirst = t;
                }
            }
            else
            {
                // Exiting halfspace
                if (t < pointlast)
                {
                    normalLast = normal;
                    pointlast = t;
                }
            }
            // Exit with “no intersection” if intersection becomes empty
            if (pointfirst > pointlast)
                return 0;
        }
    }

    // The segment intersects the polyhedron, return first and last point
    // For polygedron there are always will be only 2 points
    float length = glm::length(direction);
    if (pointfirst < 0.0f)
    {
        // Ray starts inside the convex hull; return only the exit point
        newPoints[0] = PhysicsBodyPoint({nullptr, ray.a + pointlast * direction, normalLast, pointlast * length});
        return 1;
    }
    else
    {
        // Ray intersects the convex hull at two points
        newPoints[0] = PhysicsBodyPoint({nullptr, ray.a + pointfirst * direction, normalFirst, pointfirst * length});
        newPoints[1] = PhysicsBodyPoint({nullptr, ray.a + pointlast * direction, normalLast, pointlast * length});
        return 2;
    }
}

void ShapeConvex::buildHull(Mesh *mesh, int limitToNumber, float simScale)
{
    VertexDataUV *meshVertices = mesh->getVerticies()->vertexPositionUV;
    int meshVerticesAmount = mesh->getVerticiesAmount();

    this->verticiesAmount = 0;
    this->polygonsAmount = 0;

    if (limitToNumber < 4)
        limitToNumber = 4;
    if (meshVerticesAmount < 4)
    {
        Red11::getLogger()->logFileAndConsole("Convex hull requires at least 4 points");
        return;
    }
    if (limitToNumber > meshVerticesAmount)
        limitToNumber = meshVerticesAmount;

    // Build initial hull
    Vector3 *vertices = new Vector3[meshVerticesAmount];
    for (int i = 0; i < meshVerticesAmount; i++)
        vertices[i] = meshVertices[i].position * 0.001f;

    int *faceIndices = NULL;
    int nFaces;
    convhull_3d_build((ch_vertex *)vertices, meshVerticesAmount, &faceIndices, &nFaces);

    // Reduce amount of verticies
    std::vector<int> newVerticiesIndicies;
    std::vector<QuickHullFace> newFaces;
    auto getVerticeNumber = [&](int point)
    {
        for (int i = 0; i < newVerticiesIndicies.size(); i++)
        {
            if (newVerticiesIndicies[i] == point)
                return i;
        }

        newVerticiesIndicies.push_back(point);
        return static_cast<int>(newVerticiesIndicies.size()) - 1;
    };

    for (int i = 0; i < nFaces; i++)
    {
        QuickHullFace hullFace;
        hullFace.vertices[0] = getVerticeNumber(faceIndices[i * 3]);
        hullFace.vertices[1] = getVerticeNumber(faceIndices[i * 3 + 1]);
        hullFace.vertices[2] = getVerticeNumber(faceIndices[i * 3 + 2]);
        newFaces.push_back(hullFace);
    }

    std::vector<Vector3> newVerticies;
    for (int i = 0; i < newVerticiesIndicies.size(); i++)
        newVerticies.push_back(vertices[newVerticiesIndicies[i]] / 0.001f);

    // reducing amount of verticies
    while (newVerticies.size() > limitToNumber)
    {
        int a = newFaces[0].vertices[0], b = newFaces[0].vertices[1];
        float minLength = glm::length(newVerticies[a] - newVerticies[b]);

        for (const auto &face : newFaces)
        {
            for (int i = 0; i < 3; i++)
            {
                int v1 = face.vertices[i];
                int v2 = face.vertices[(i + 1) % 3];

                float length = glm::length(newVerticies[v1] - newVerticies[v2]);
                if (length < minLength)
                {
                    a = v1;
                    b = v2;
                    minLength = length;
                }
            }
        }

        // newVerticies[a] = (newVerticies[a] + newVerticies[b]) * 0.5f;
        for (auto &face : newFaces)
        {
            for (int i = 0; i < 3; i++)
            {
                if (face.vertices[i] == b)
                    face.vertices[i] = a;
            }
        }
        newVerticies.erase(newVerticies.begin() + b);

        for (auto &face : newFaces)
        {
            for (int i = 0; i < 3; i++)
            {
                if (face.vertices[i] > b)
                    face.vertices[i]--;
            }
        }

        // Remove degenerate faces
        newFaces.erase(
            std::remove_if(newFaces.begin(), newFaces.end(),
                           [](const QuickHullFace &face)
                           {
                               return face.vertices[0] == face.vertices[1] ||
                                      face.vertices[1] == face.vertices[2] ||
                                      face.vertices[2] == face.vertices[0];
                           }),
            newFaces.end());
    }

    delete[] vertices;
    free(faceIndices);

    // reduced convex hull might not be convex anymore so rebuild convex hull around reduced convex
    vertices = new Vector3[newVerticies.size()];
    for (int i = 0; i < newVerticies.size(); i++)
        vertices[i] = newVerticies[i];
    convhull_3d_build((ch_vertex *)vertices, newVerticies.size(), &faceIndices, &nFaces);

    // Set the final data
    this->verticiesAmount = newVerticies.size();
    this->verticies = new Vector3[this->verticiesAmount];
    for (int i = 0; i < this->verticiesAmount; i++)
        this->verticies[i] = newVerticies[i] * simScale;

    this->polygonsAmount = nFaces;
    this->polygons = new HullPolygon[this->polygonsAmount];
    for (int i = 0; i < nFaces; i++)
    {
        this->polygons[i].index = i;
        this->polygons[i].normal = calculateNormal(
            newVerticies[faceIndices[i * 3]],
            newVerticies[faceIndices[i * 3 + 1]],
            newVerticies[faceIndices[i * 3 + 2]]);

        this->polygons[i].pointsAmount = 3;
        for (int v = 0; v < 3; v++)
            this->polygons[i].points[v] = faceIndices[i * 3 + v];
    }

    delete[] vertices;
    free(faceIndices);
}
