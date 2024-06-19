// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentMeshGroup.h"

ComponentMeshGroup::ComponentMeshGroup()
{
}

ComponentMeshGroup::~ComponentMeshGroup()
{
    destroyList();
}

void ComponentMeshGroup::onRender(Camera *camera, Renderer *renderer)
{
    for (auto &it : list)
    {
        if (it->bIsSkinned)
        {
            boneTransforms.clear();
            for (auto &boneIt : bonesList)
                boneTransforms.push_back(BoneTransform(boneIt->getModelMatrix(), boneIt->getNamePointer()));
            renderer->renderMeshSkinned(camera, it->getMesh(), material, it->getModelMatrix(), &boneTransforms);
        }
        else
        {
            if (it->getMesh())
                renderer->renderMesh(camera, it->getMesh(), material, it->getModelMatrix());
        }
    }
}

void ComponentMeshGroup::onRenderQueue(Renderer *renderer)
{
    for (auto &it : list)
    {
        if (it->getMesh())
        {
            if (it->bIsSkinned)
            {
                boneTransforms.clear();
                for (auto &boneIt : bonesList)
                    boneTransforms.push_back(BoneTransform(boneIt->getModelMatrix(), boneIt->getNamePointer()));
                renderer->queueMeshSkinned(it->getMesh(), material, it->getModelMatrix(), &boneTransforms);
            }
            else
            {
                renderer->queueMesh(it->getMesh(), material, it->getModelMatrix());
            }
        }
    }

    if (bBonesView)
    {
        for (auto &it : list)
        {
            if (it->isBone() && it->getParent() && it->getParent()->isBone())
            {
                Vector3 from = Vector3(*it->getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
                Vector3 to = Vector3(*it->getParent()->getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
                renderer->queueLine(from, to, Color(0.8f, 0.8f, 0.8f, 1.0f));
            }
        }
    }
}

void ComponentMeshGroup::onProcess(float delta)
{
    // gather total weight
    bool hasAnimations = false;
    for (auto &track : tracks)
    {
        track->process(delta);
        if (track->isPlaying())
            hasAnimations = true;
    }

    if (!hasAnimations)
    {
        // no animations playing
        for (auto &it : list)
        {
            it->setPosition(it->initialPosition);
            it->setRotation(it->initialRotation);
            it->setScale(it->initialScale);
        }
    }
    else
    {
        for (auto &node : list)
        {
            // Some nodes may not exist in some tracks so we need to calc local one
            float totalWeightNode = 0.0f;
            for (auto &track : tracks)
            {
                if (track->isPlaying())
                    totalWeightNode += track->getNodeWeight(node->getNamePointer());
            }

            float initialTake = fmaxf(0.0f, 1.0f - totalWeightNode);
            totalWeightNode += initialTake;

            Vector3 position = initialTake > 0.0f ? node->initialPosition * initialTake : Vector3(0.0f);
            Quat rotation = node->initialRotation;
            Vector3 scale = initialTake > 0.0f ? node->initialScale * initialTake : Vector3(0.0f);

            for (auto &track : tracks)
                track->addTransformation(totalWeightNode, node->getNamePointer(), &position, &rotation, &scale);

            // printf("%s - %f %f %f\n", node->getNamePointer()->c_str(), position.x, position.y, position.z);

            node->setPosition(position);
            node->setRotation(rotation);
            node->setScale(scale);
        }
    }
}

void ComponentMeshGroup::setDebugBonesView(bool bState)
{
    bBonesView = bState;
}

void ComponentMeshGroup::setMeshList(std::vector<MeshObject *> *newList)
{
    destroyList();

    // create objects
    for (auto &it : *newList)
    {
        AnimationMeshObject *newObj = new AnimationMeshObject(it);
        newObj->initialPosition = it->getPosition();
        newObj->initialRotation = it->getRotation();
        newObj->initialScale = it->getScale();
        newObj->setName(*it->getNamePointer());
        list.push_back(newObj);
        if (newObj->isBone())
            bonesList.push_back(newObj);
    }

    // relink parents
    int iChild = 0;
    for (auto &child : *newList)
    {
        if (child->getParent())
        {
            int iParent = 0;
            for (auto &parent : *newList)
            {
                if (parent == child->getParent())
                {
                    list.at(iChild)->setParent(list.at(iParent));
                    break;
                }
                iParent++;
            }
        }
        iChild++;
    }

    // link ones without parent to component
    for (auto &it : list)
        if (!it->getParent())
            it->setEntityParent(this);
}

AnimationTrack *ComponentMeshGroup::createAnimationTrack(Animation *animation)
{
    auto animTrack = new AnimationTrack(animation);
    tracks.push_back(animTrack);
    return animTrack;
}

void ComponentMeshGroup::setMaterial(Material *material)
{
    this->material = material;
}

void ComponentMeshGroup::destroyList()
{
    for (auto &it : list)
        delete it;
    list.clear();
    bonesList.clear();
}