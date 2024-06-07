// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "component.h"
#include "data/animation/animation.h"
#include "data/animation/animationTrack.h"
#include "data/meshObject.h"
#include "utils/utils.h"

class AnimationMeshObject : public MeshObject
{
public:
    AnimationMeshObject(MeshObject *obj) : MeshObject(obj->getMesh())
    {
        initialPosition = obj->getPosition();
        initialRotation = obj->getRotation();
        initialScale = obj->getScale();
    }

    Vector3 initialPosition;
    Quat initialRotation;
    Vector3 initialScale;
};

class ComponentMeshGroup : public Component
{
public:
    EXPORT ComponentMeshGroup();
    EXPORT ~ComponentMeshGroup();

    EXPORT void onRender(Renderer *renderer) override final;
    EXPORT void onRenderQueue(Renderer *renderer) override final;
    EXPORT void onProcess(float delta) override final;

    // all MeshObjects will be recreated with meshes and structure for this mesh group component
    // it can be changed freely locally in this component
    EXPORT void setMeshList(std::vector<MeshObject *> *newList);
    EXPORT AnimationTrack *createAnimationTrack(Animation *animation);

    EXPORT void setMaterial(Material *material);
    inline Material *getMaterial() { return material; }

    EXPORT MeshObject *getObjectByIndex(unsigned int index);

protected:
    std::vector<AnimationMeshObject *> list;
    std::vector<AnimationTrack *> tracks;
    Material *material = nullptr;

    void destroyList();
};