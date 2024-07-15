// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "utils/utils.h"

class Entity
{
public:
    inline void setPosition(Vector3 v)
    {
        this->position = v;
        bIsTransformationDirty = true;
    }

    inline void setPosition(Vector2 v)
    {
        this->position = Vector3(v.x, v.y, 0.0f);
        bIsTransformationDirty = true;
    }

    inline void setPosition(float x, float y, float z)
    {
        this->position = Vector3(x, y, z);
        bIsTransformationDirty = true;
    }

    inline void setPosition(float x, float y)
    {
        this->position = Vector3(x, y, 0.0f);
        bIsTransformationDirty = true;
    }

    inline void setPositionX(float value)
    {
        this->position.x = value;
        bIsTransformationDirty = true;
    }

    inline void setPositionY(float value)
    {
        this->position.y = value;
        bIsTransformationDirty = true;
    }

    inline void setPositionZ(float value)
    {
        this->position.z = value;
        bIsTransformationDirty = true;
    }

    inline void translate(float x, float y, float z)
    {
        this->position += Vector3(x, y, z);
        bIsTransformationDirty = true;
    }

    inline void translate(float x, float y)
    {
        this->position += Vector3(x, y, 0.0f);
        bIsTransformationDirty = true;
    }

    inline void translate(Vector3 v)
    {
        this->position += v;
        bIsTransformationDirty = true;
    }

    inline void translate(Vector2 v)
    {
        this->position += Vector3(v.x, v.y, 0.0f);
        bIsTransformationDirty = true;
    }

    inline Vector3 getPosition() { return position; };

    inline void setRotation(Vector3 r)
    {
        this->rotation = Quat(r);
        bIsTransformationDirty = true;
    }
    inline void setRotation(Quat r)
    {
        this->rotation = r;
        bIsTransformationDirty = true;
    }
    inline void setRotation(float z)
    {
        this->rotation = Quat(Vector3(0.0f, 0.0f, z));
        bIsTransformationDirty = true;
    }
    inline void rotate(float z)
    {
        this->rotation *= Quat(Vector3(0.0f, 0.0f, z));
        bIsTransformationDirty = true;
    }
    inline void rotate(Vector3 r)
    {
        this->rotation *= Quat(r);
        bIsTransformationDirty = true;
    }
    inline void rotate(Quat r)
    {
        this->rotation *= r;
        bIsTransformationDirty = true;
    }
    inline void rotateByNormal(Vector3 normal, Vector3 up = Vector3(0.0f, 1.0f, 0.0f))
    {
        this->rotation = glm::rotation(up, normal);
        bIsTransformationDirty = true;
    }

    inline Quat getRotation() { return rotation; };
    inline Vector3 getRotationEuler() { return glm::eulerAngles(rotation); };

    inline void setScale(Vector3 v)
    {
        this->scale = v;
        bIsTransformationDirty = true;
    }
    inline void setScale(Vector2 v)
    {
        this->scale = Vector3(v.x, v.y, 1.0f);
        bIsTransformationDirty = true;
    }
    inline void setScale(float x, float y, float z)
    {
        this->scale = Vector3(x, y, z);
        bIsTransformationDirty = true;
    }
    inline void setScale(float x, float y)
    {
        this->scale = Vector3(x, y, 1.0f);
        bIsTransformationDirty = true;
    }
    inline void setScale(float xyz)
    {
        this->scale = Vector3(xyz, xyz, xyz);
        bIsTransformationDirty = true;
    }
    inline Vector3 getScale() { return scale; };

    inline bool isTransformationDirty() { return bIsTransformationDirty; }

    inline int getTIteration()
    {
        return tIteration;
    }

    EXPORT Matrix4 *getModelMatrix();

protected:
    Vector3 position = Vector3(0.0f);
    Quat rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
    Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
    bool bIsTransformationDirty = true;

    Matrix4 mModelLocal;
    Matrix4 mModelWithParent;

    Entity *transformationParent = nullptr;
    int tIteration = 0;
    int tLastParentIteration = 0;

    inline void setTransformationParent(Entity *transformationParent)
    {
        this->transformationParent = transformationParent;
        bIsTransformationDirty = true;
    }
};
