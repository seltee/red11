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
    AnimationMeshObject(MeshObject *obj) : MeshObject(obj->getMesh(), obj->isBone())
    {
        initialPosition = obj->getPosition();
        initialRotation = obj->getRotation();
        initialScale = obj->getScale();

        if (obj->getMesh() && obj->getMesh()->getDeforms()->size() > 0)
        {
            bIsSkinned = true;
        }
    }

    Vector3 initialPosition;
    Quat initialRotation;
    Vector3 initialScale;

    bool bIsSkinned = false;
};

class ComponentMeshGroup : public Component
{
public:
    EXPORT ComponentMeshGroup();
    EXPORT ~ComponentMeshGroup();

    EXPORT void onRender(Camera *camera, Renderer *renderer) override final;
    EXPORT void onRenderQueue(Renderer *renderer) override final;
    EXPORT void onProcess(float delta) override final;

    EXPORT void setDebugBonesView(bool bState);

    // all MeshObjects will be recreated with meshes and structure for this mesh group component
    // it can be changed freely locally in this component
    EXPORT void setMeshList(std::vector<MeshObject *> *newList);
    EXPORT AnimationTrack *createAnimationTrack(Animation *animation);

    EXPORT void setMaterial(Material *material);
    inline Material *getMaterial() { return material; }

protected:
    std::vector<AnimationMeshObject *> list;
    std::vector<AnimationMeshObject *> bonesList;
    std::vector<AnimationTrack *> tracks;
    std::vector<BoneTransform> boneTransforms;
    Material *material = nullptr;
    bool bBonesView = false;

    void destroyList();
};