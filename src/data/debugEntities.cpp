#include "debugEntities.h"
#include "red11.h"

DebugEntities::DebugEntities()
{
    debugCubeMesh = Red11::getMeshBuilder()->createCube(1.0f);
    debugCubeMesh->setCastsShadow(false);
    matPositive = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.0f, 0.8f, 0.0f));
    matNegative = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.8f, 0.0f, 0.0f));
    matNormal = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.0f, 0.0f, 0.8f));
    matPhysics = new MaterialSimple(Color(0.0, 0.0, 0.0), Color(0.960f, 0.843f, 0.182f));
}

void DebugEntities::makeDebugCubeIntoLine(ComponentMesh *mesh, const Vector3 &pA, const Vector3 &pB)
{
    Vector3 center = (pA + pB) * 0.5f;
    Vector3 difference = pB - pA;
    Vector3 normal = glm::normalize(difference);

    float y = atan2f(normal.z, normal.x);
    float len = sqrtf(normal.x * normal.x + normal.z * normal.z);
    float x = atan2(len, normal.y);

    mesh->setPosition(center);
    mesh->setScale(Vector3(debugScale, debugScale, glm::length(difference)));
    mesh->setRotation(Vector3(CONST_PI / 2 - x, -y - CONST_PI / 2.0f, 0.0f));
}

void DebugEntities::makeDebugCubeIntoPoint(ComponentMesh *mesh, const Vector3 &center, const Vector3 *lookAt)
{
    mesh->setPosition(center);
    mesh->setScale(Vector3(debugScale * 4.0f));
    if (lookAt)
        mesh->lookAt(*lookAt);
}

Matrix4 DebugEntities::makeDebugCubeIntoLineMatrix(const Vector3 &pA, const Vector3 &pB)
{
    Entity entity;
    Vector3 center = (pA + pB) * 0.5f;
    Vector3 difference = pB - pA;
    Vector3 normal = glm::normalize(difference);

    float y = atan2f(normal.z, normal.x);
    float len = sqrtf(normal.x * normal.x + normal.z * normal.z);
    float x = atan2(len, normal.y);

    entity.setPosition(center);
    entity.setScale(Vector3(debugScale, debugScale, glm::length(difference)));
    entity.setRotation(Vector3(CONST_PI / 2 - x, -y - CONST_PI / 2.0f, 0.0f));

    return entity.getModelMatrix();
}

Matrix4 DebugEntities::makeDebugCubeIntoPointMatrix(const Vector3 &center, const Vector3 *lookAt)
{
    Entity entity;

    entity.setPosition(center);
    entity.setScale(Vector3(debugScale * 4.0f));
    if (lookAt)
        entity.lookAt(*lookAt);

    return entity.getModelMatrix();
}