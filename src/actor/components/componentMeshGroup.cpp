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

void ComponentMeshGroup::onRender(Renderer *renderer)
{
    for (auto &it : list)
        renderer->renderMesh(it->getMesh(), material, it->getModelMatrix());
}

void ComponentMeshGroup::onRenderQueue(Renderer *renderer)
{
    for (auto &it : list)
    {
        renderer->queueMesh(it->getMesh(), material, it->getModelMatrix());
    }
}

void ComponentMeshGroup::onProcess(float delta)
{
    // gather total weight
    float totalWeight = 0.0f;
    for (auto &track : tracks)
    {
        track->process(delta);
        if (track->isPlaying())
            totalWeight += track->getWeight();
    }

    if (totalWeight == 0.0f)
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
        float initialTake = fmaxf(0.0f, 1.0f - totalWeight);

        for (auto &node : list)
        {
            Vector3 position = initialTake > 0.0f ? node->initialPosition * initialTake : Vector3(0.0f);
            Quat rotation = initialTake > 0.0f ? node->initialRotation * initialTake : Quat();
            Vector3 scale = initialTake > 0.0f ? node->initialScale * initialTake : Vector3(0.0f);

            for (auto &track : tracks)
            {
                track->addTransformation(totalWeight, node->getNamePointer(), &position, &rotation, &scale);
            }

            node->setPosition(position);
            node->setRotation(rotation);
            node->setScale(scale);
        }
    }
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
    }

    // relink parents
    for (auto &it : *newList)
    {
        MeshObject *parent = it->getParent();
        if (parent)
        {
            unsigned int id = it->getMesh()->getIndex();
            unsigned int parentId = parent->getMesh()->getIndex();

            MeshObject *localObject = getObjectByIndex(id);
            MeshObject *parentLocalObject = getObjectByIndex(parentId);

            if (localObject && parentLocalObject)
                localObject->setParent(parentLocalObject);
        }
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

MeshObject *ComponentMeshGroup::getObjectByIndex(unsigned int index)
{
    for (auto &it : list)
        if (it->getMesh()->getIndex() == index)
            return it;
    return nullptr;
}

void ComponentMeshGroup::destroyList()
{
    for (auto &it : list)
        delete it;
    list.clear();
}