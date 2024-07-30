// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentMeshGroup.h"
#include <utils/glm/gtx/matrix_decompose.hpp>

ComponentMeshGroup::ComponentMeshGroup()
{
}

ComponentMeshGroup::~ComponentMeshGroup()
{
    destroyList();
    if (material)
        material->removeUser();
}

void ComponentMeshGroup::onRenderQueue(Renderer *renderer)
{
    for (auto &it : list)
    {
        Mesh *mesh = it->getMesh();
        if (mesh)
        {
            if (it->bIsSkinned)
            {
                boneTransforms.clear();
                for (auto &boneIt : bonesList)
                {
                    Deform *deform = mesh->getDeformByName(*boneIt->getNamePointer());
                    if (deform)
                        boneTransforms.push_back(BoneTransform(boneIt->getModelMatrix(), deform));
                }
                renderer->queueMeshSkinned(mesh, material, it->getModelMatrix(), &boneTransforms);
            }
            else
            {
                renderer->queueMesh(mesh, material, it->getModelMatrix());
            }
        }
    }

    if (bViewBones || bViewCullingSpheres)
    {
        for (auto &it : list)
        {
            if (bViewBones && it->isBone() && it->getParent() && it->getParent()->isBone())
            {
                Vector3 from = Vector3(*it->getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
                Vector3 to = Vector3(*it->getParent()->getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
                renderer->queueLine(from, to, Color(0.2f, 0.8f, 0.2f, 1.0f));
            }
            if (bViewBones && it->isBone() && it->getParent() && it->getParent()->isBone())
            {
                for (auto &meshIt : list)
                {
                    if (meshIt->getMesh())
                    {
                        float scale = findMaxScale(extractScale(*it->getModelMatrix()));
                        Deform *deform = meshIt->getMesh()->getDeformByName(*it->getNamePointer());
                        if (deform)
                        {
                            float radius = deform->getCullingRadius() * scale;
                            Vector3 from = Vector3(*it->getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
                            renderer->queueLine(from, from + Vector3(0, radius, 0), Color(0.8f, 0.2f, 0.2f, 1.0f));
                            renderer->queueLine(from, from + Vector3(0, -radius, 0), Color(0.8f, 0.2f, 0.2f, 1.0f));
                            renderer->queueLine(from, from + Vector3(radius, 0, 0), Color(0.8f, 0.2f, 0.2f, 1.0f));
                            renderer->queueLine(from, from + Vector3(-radius, 0, 0), Color(0.8f, 0.2f, 0.2f, 1.0f));
                            renderer->queueLine(from, from + Vector3(0, 0, radius), Color(0.8f, 0.2f, 0.2f, 1.0f));
                            renderer->queueLine(from, from + Vector3(0, 0, -radius), Color(0.8f, 0.2f, 0.2f, 1.0f));
                        }
                    }
                }
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

void ComponentMeshGroup::setDebugBonesView(bool bViewBones, bool bViewCullingSpheres)
{
    this->bViewBones = bViewBones;
    this->bViewCullingSpheres = bViewCullingSpheres;
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
    if (this->material)
        this->material->removeUser();

    this->material = material;
    if (this->material)
        this->material->addUser();
}

void ComponentMeshGroup::destroyList()
{
    for (auto &it : list)
        delete it;
    list.clear();
    bonesList.clear();
}