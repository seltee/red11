#pragma once
#include "data/mesh.h"
#include "data/material/materialSimple.h"
#include "actor/components/componentMesh.h"
#include "utils/utils.h"

class DebugEntities
{
public:
    EXPORT DebugEntities();

    EXPORT void makeDebugCubeIntoLine(ComponentMesh *mesh, const Vector3 &pA, const Vector3 &pB);
    EXPORT void makeDebugCubeIntoPoint(ComponentMesh *mesh, const Vector3 &center, const Vector3 *lookAt);

    EXPORT Matrix4 makeDebugCubeIntoLineMatrix(const Vector3 &pA, const Vector3 &pB);
    EXPORT Matrix4 makeDebugCubeIntoPointMatrix(const Vector3 &center, const Vector3 *lookAt);

    Mesh *debugCubeMesh = nullptr;
    MaterialSimple *matPositive = nullptr;
    MaterialSimple *matNegative = nullptr;
    MaterialSimple *matPhysics = nullptr;

    float debugScale = 0.004f;
};