// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#define _CRT_SECURE_NO_WARNINGS

#include "renderer/renderer.h"
#include "loaderFBX.h"
#include "utils/image/stb_image.h"
#include <stdio.h>
#include <algorithm>

LoaderFBX::LoaderFBX()
{
}

bool LoaderFBX::loadFBXFile(std::string path, std::vector<MeshObject *> *meshObjectsList, std::vector<Animation *> *animationsList)
{
    printf("Loading FBX %s ...\n", path.c_str());
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
        return false;

    // =======
    // Checking header
    // =======
    char buff[21];
    fread(buff, 21, 1, file);
    if (std::strcmp(buff, "Kaydara FBX Binary  ") != 0)
    {
        // logger->logff("File isn't FBX %s\n", path.c_str());
        printf("File isn't FBX %s\n", path.c_str());
        return false;
    }

    fread(buff, 2, 1, file);
    if (buff[0] != 0x1A || buff[1] != 0x00)
    {
        // logger->logff("Wrong FBX code %s\n", path.c_str());
        printf("Wrong FBX code %s\n", path.c_str());
        return false;
    }

    unsigned int version;
    fread(&version, 4, 1, file);
    if (version != 7400)
    {
        if (version < 7400)
        {
            // logger->logff("Warning! FBX version is less than 7.4, errors of reading may happen, %s\n", path.c_str());
            printf("Warning! FBX version is less than 7.4, errors of reading may happen, %s\n", path.c_str());
        }
        else
        {
            // logger->logff("Versions of FBX higher than 7.4 are unsupported, failed to load %s\n", path.c_str());
            printf("Versions of FBX higher than 7.4 are unsupported, failed to load %s\n", path.c_str());
            return false;
        }
    }

    // =======
    // Loading the file, headers are fine
    // =======

    std::vector<FBXModel *> models;
    std::vector<FBXAnimationStack *> animStacks;
    std::vector<FBXAnimationLayer *> animLayers;
    std::vector<FBXAnimationCurveNode *> animCurveNodes;
    std::vector<FBXAnimationCurve *> animCurves;
    std::vector<FBXDeform *> deforms;
    std::vector<FBXGeometry *> geometries;
    std::vector<FBXAttribute *> attributes;

    std::vector<FBXNode *> nodes;

    // loop through the nodes
    while (1)
    {
        FBXNode *node = new FBXNode(0, file);
        node->process();
        if (node->bIsZero)
            break;
        if (node->bMarkedToRemove)
            delete node;
        else
            nodes.push_back(node);
    }

    FBXNode *objects = nullptr;
    FBXNode *connections = nullptr;

    for (auto &it : nodes)
    {
        // it->print();
        if (it->isName("Objects"))
            objects = it;
        if (it->isName("Connections"))
            connections = it;
    }

    if (!objects || !connections)
    {
        // logger->logff("Objects or connections weren't found for %s\n", path.c_str());
        printf("Objects or connections weren't found for %s\n", path.c_str());
        return false;
    }

    // collecting geometry and models info
    for (auto &it : objects->children)
    {
        if (it->isName("Geometry"))
        {
            FBXGeometry *geometry = new FBXGeometry(it);
            geometries.push_back(geometry);
            continue;
        }

        if (it->isName("Model"))
        {
            FBXModel *model = new FBXModel(it);
            models.push_back(model);
            continue;
        }

        if (it->isName("AnimationStack"))
        {
            auto newAnimStack = new FBXAnimationStack(it);
            animStacks.push_back(newAnimStack);
            continue;
        }

        if (it->isName("AnimationLayer"))
        {
            auto newAnimLayer = new FBXAnimationLayer(it);
            animLayers.push_back(newAnimLayer);
            continue;
        }

        if (it->isName("AnimationCurveNode"))
        {
            auto newAnimCurveNode = new FBXAnimationCurveNode(it);
            animCurveNodes.push_back(newAnimCurveNode);
            continue;
        }

        if (it->isName("AnimationCurve"))
        {
            auto newAnimCurve = new FBXAnimationCurve(it);
            animCurves.push_back(newAnimCurve);
            continue;
        }

        // Bones
        if (it->isName("NodeAttribute"))
        {
            FBXAttribute *attribute = new FBXAttribute(it);
            attributes.push_back(attribute);
        }

        if (it->isName("Pose"))
        {
        }

        if (it->isName("Deformer"))
        {
            FBXDeform *newDeform = new FBXDeform(it);
            deforms.push_back(newDeform);
        }
    }

    // Connecting models, geometry and animations
    for (auto &it : connections->children)
    {
        unsigned long long indexFrom = *reinterpret_cast<unsigned long long *>(it->bindedData.at(1).data);
        unsigned long long indexTo = *reinterpret_cast<unsigned long long *>(it->bindedData.at(2).data);

        FBXModel *foundModel = getModelById(indexFrom, models);
        if (foundModel)
        {
            if (indexTo)
            {
                FBXModel *toModel = getModelById(indexTo, models);
                FBXDeform *toDeform = getDeformById(indexTo, deforms);
                if (toModel)
                    foundModel->parentId = indexTo;
                if (toDeform)
                    foundModel->parentDeformId = indexTo;
            }
            continue;
        }

        FBXGeometry *foundGeometry = getGeometryById(indexFrom, geometries);
        if (foundGeometry)
        {
            foundModel = getModelById(indexTo, models);
            if (foundModel)
                foundModel->geometry = foundGeometry;
            continue;
        }

        FBXAnimationLayer *layer = getLayerById(indexFrom, animLayers);
        if (layer)
        {
            FBXAnimationStack *stack = getStackById(indexTo, animStacks);
            if (stack)
            {
                stack->linkLayer(layer);
            }
            continue;
        }

        FBXAnimationCurveNode *curveNode = getCurveNodeById(indexFrom, animCurveNodes);
        if (curveNode)
        {
            FBXModel *targetModel = getModelById(indexTo, models);
            FBXAnimationLayer *targetLayer = getLayerById(indexTo, animLayers);
            if (targetModel)
            {
                targetModel->curveNodes.push_back(curveNode);
                curveNode->addAffectedModel(targetModel);
            }
            if (targetLayer)
            {
                targetLayer->linkCurveNode(curveNode);
            }
            continue;
        }

        FBXAnimationCurve *curve = getCurveById(indexFrom, animCurves);
        if (curve)
        {
            FBXAnimationCurveNode *curveNode = getCurveNodeById(indexTo, animCurveNodes);
            if (curveNode)
            {
                curveNode->linkCurve(curve, it);
            }
            continue;
        }

        FBXDeform *deform = getDeformById(indexFrom, deforms);
        if (deform)
        {
            FBXDeform *deformParent = getDeformById(indexTo, deforms);
            if (deformParent)
            {
                deformParent->addChild(deform);
                deform->setParent(deformParent);
            }
            else
            {
                FBXGeometry *deformGeometry = getGeometryById(indexTo, geometries);
                if (deformGeometry)
                    deformGeometry->addDeformer(deform);
                else
                    printf("Uknown parent\n");
            }

            continue;
        }

        FBXAttribute *attribute = getAttributeById(indexFrom, attributes);
        if (attribute)
        {
            FBXModel *targetModel = getModelById(indexTo, models);
            if (targetModel)
                targetModel->addAttribute(attribute);
            else
                printf("Attribute node unknown binding %llu to %llu\n", indexFrom, indexTo);

            continue;
        }

        printf("Unknown link %llu to %llu\n", indexFrom, indexTo);
    }

    // printAnimationStructure();

    // setting up animation
    if (!animStacks.empty())
    {
        auto animNames = getAnimationNames(animLayers);
        for (auto &animName : animNames)
        {
            Animation *newAnimation = new Animation(animName);
            animationsList->push_back(newAnimation);

            for (auto &layer : animLayers)
            {
                if (layer->name == animName)
                {
                    std::vector<unsigned long long> timestamps;
                    gatherTimeStamps(layer, &timestamps);

                    for (auto &timeStamp : timestamps)
                    {
                        std::vector<FBXAnimationBinding> animBindings;
                        float floatTimeStamp = static_cast<float>(timeStamp / FBXTimeToMs) / 1000.0f;

                        for (auto &curveNode : layer->curveNodes)
                        {
                            Vector3 def = curveNode->defaultValue;
                            FBXAnimationCurve *curveX = curveNode->getXCurve();
                            FBXAnimationCurve *curveY = curveNode->getYCurve();
                            FBXAnimationCurve *curveZ = curveNode->getZCurve();

                            Vector3 out = def;

                            if (curveX)
                                out.x = getCurveLerped(curveX, floatTimeStamp);
                            if (curveY)
                                out.y = getCurveLerped(curveY, floatTimeStamp);
                            if (curveZ)
                                out.z = getCurveLerped(curveZ, floatTimeStamp);

                            // Make sure we have all have names in the list of transforms
                            for (auto &model : curveNode->affectedModels)
                            {
                                bool bFound = false;
                                for (auto &binding : animBindings)
                                {
                                    if (binding.modelName == model->getName())
                                    {
                                        bFound = true;
                                        break;
                                    }
                                }
                                if (!bFound)
                                {
                                    FBXAnimationBinding binding;
                                    binding.modelName = model->getName();
                                    binding.keyTransform = AnimationKeyTranform({floatTimeStamp,
                                                                                 model->position,
                                                                                 model->rotation,
                                                                                 model->scale});
                                    animBindings.push_back(binding);
                                }
                            }

                            // affect transforms in list for this curve
                            for (auto &anim : animBindings)
                            {
                                if (curveNode->hasModelName(anim.modelName))
                                {
                                    if (curveNode->type == FBXAnimationCurveNodeType::Position)
                                        anim.keyTransform.position = out;
                                    if (curveNode->type == FBXAnimationCurveNodeType::Scale)
                                        anim.keyTransform.scale = out;
                                    if (curveNode->type == FBXAnimationCurveNodeType::Rotation)
                                        anim.keyTransform.rotation = (out / 180.0f) * CONST_PI;
                                }
                            }
                        }

                        // each timestamp we add transformation to anim bindings
                        for (auto &animBinding : animBindings)
                        {
                            // printf("ANIM %f - %s\n", animBinding.keyTransform.timeStamp, animBinding.modelName.c_str());
                            // printf("p %f %f %f\n", animBinding.keyTransform.position.x, animBinding.keyTransform.position.y, animBinding.keyTransform.position.z);
                            // printf("r %f %f %f\n", animBinding.keyTransform.rotation.x, animBinding.keyTransform.rotation.y, animBinding.keyTransform.rotation.z);
                            // printf("s %f %f %f\n", animBinding.keyTransform.scale.x, animBinding.keyTransform.scale.y, animBinding.keyTransform.scale.z);
                            auto animTarget = newAnimation->createAnimationTarget(animBinding.modelName);
                            animTarget->addKey(animBinding.keyTransform);
                        }
                    }
                }

                newAnimation->recalcAnimationLength();
            }
        }
    }

    if (meshObjectsList)
    {
        // Create Mesh Objects
        for (auto &model : models)
        {
            MeshObject *newObject = new MeshObject(model->geometry ? model->geometry->getMesh() : nullptr, model->isLimb());
            newObject->setPosition(model->position);
            newObject->setRotation(model->rotation);
            newObject->setScale(model->scale);
            newObject->setName(model->getName());

            model->meshObject = newObject;

            meshObjectsList->push_back(newObject);

            /*
                printf("- %s (%llu)\n", model->getName().c_str(), model->id);
                if (model->geometry)
                    printf("  Has geometry\n");
                if (model->isLimb())
                    printf("  Is limb\n");
                if (model->parentId)
                    printf("  Parent %ll u\n", model->parentId);
            */
        }

        // Parent Mesh Objects
        for (auto &model : models)
        {
            if (model->parentId && model->meshObject)
            {
                auto parentModel = getModelById(model->parentId, models);
                if (parentModel && parentModel->meshObject)
                {
                    model->meshObject->setParent(parentModel->meshObject);
                }
            }
            /*
            if (model->parentDeformId)
            {
                auto parentDeform = getDeformById(model->parentDeformId, deforms);
                if (parentDeform)
                {
                    FBXModel *deformModel = getModelByDeform(parentDeform, models);
                    // if (deformModel && deformModel->meshObject)
                    //     model->meshObject->setParent(deformModel->meshObject);
                }
            }
            */
        }
    }

    return true;
}

void LoaderFBX::printAnimationStructure(std::vector<FBXAnimationStack *> &animStacks)
{
    for (auto &stack : animStacks)
    {
        printf("Stack %llu\n", stack->id);
        for (auto &layer : stack->layers)
        {
            printf("-Layer %s\n", layer->name.c_str());
            for (auto &node : layer->curveNodes)
            {
                printf("--Node %s\n", node->getTypeName());
                for (auto &curve : node->curves)
                {
                    printf("---Curve %llu, %c\n", curve.curve->id, curve.axis);
                    for (int i = 0; i < curve.curve->keysCount; i++)
                    {
                        unsigned long long ms = curve.curve->keys[i].keyTime / FBXTimeToMs;
                        printf("----Key %llums - %f\n", ms, curve.curve->keys[i].keyValue);
                    }
                }
            }
        }
    }
}

FBXModel *LoaderFBX::getModelById(unsigned long long id, std::vector<FBXModel *> &models)
{
    for (auto &it : models)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationStack *LoaderFBX::getStackById(unsigned long long id, std::vector<FBXAnimationStack *> &animStacks)
{
    for (auto &it : animStacks)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationLayer *LoaderFBX::getLayerById(unsigned long long id, std::vector<FBXAnimationLayer *> &animLayers)
{
    for (auto &it : animLayers)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationCurveNode *LoaderFBX::getCurveNodeById(unsigned long long id, std::vector<FBXAnimationCurveNode *> &animCurveNodes)
{
    for (auto &it : animCurveNodes)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAnimationCurve *LoaderFBX::getCurveById(unsigned long long id, std::vector<FBXAnimationCurve *> &animCurves)
{
    for (auto &it : animCurves)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXDeform *LoaderFBX::getDeformById(unsigned long long id, std::vector<FBXDeform *> &deforms)
{
    for (auto &it : deforms)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXGeometry *LoaderFBX::getGeometryById(unsigned long long id, std::vector<FBXGeometry *> &geometries)
{
    for (auto &it : geometries)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXAttribute *LoaderFBX::getAttributeById(unsigned long long id, std::vector<FBXAttribute *> &attributes)
{
    for (auto &it : attributes)
    {
        if (it->id == id)
            return it;
    }
    return nullptr;
}

FBXModel *LoaderFBX::getModelByDeform(FBXDeform *deform, std::vector<FBXModel *> &models)
{
    for (auto &it : models)
    {
        if (it->geometry && it->geometry->hasDeformer(deform))
            return it;
    }
    return nullptr;
}

std::vector<std::string> LoaderFBX::getAnimationNames(std::vector<FBXAnimationLayer *> &animLayers)
{
    std::vector<std::string> animNames;
    for (auto &it : animLayers)
    {
        bool found = false;
        for (auto &animName : animNames)
        {
            if (animName == it->name)
                found = true;
        }
        if (!found)
        {
            animNames.push_back(it->name);
        }
    }
    return animNames;
}

void LoaderFBX::gatherTimeStamps(FBXAnimationLayer *layer, std::vector<unsigned long long> *timestamps)
{
    for (auto &curveNode : layer->curveNodes)
    {
        for (auto &curve : curveNode->curves)
        {
            int keysCount = curve.curve->keysCount;
            auto keys = curve.curve->keys;

            for (int i = 0; i < keysCount; i++)
            {
                auto keyTime = keys[i].keyTime;
                bool found = false;
                for (auto &time : *timestamps)
                {
                    if (time == keyTime)
                        found = true;
                }
                if (!found)
                    timestamps->push_back(keyTime);
            }
        }
    }
    std::sort(timestamps->begin(), timestamps->end());
}

float LoaderFBX::getCurveLerped(FBXAnimationCurve *curve, float time)
{
    int keysCount = curve->keysCount;
    auto keys = curve->keys;

    for (int i = 0; i < keysCount; i++)
    {
        float keyTime = static_cast<float>(keys[i].keyTime / FBXTimeToMs) / 1000.0f;
        float prevKeyTime = i > 0 ? static_cast<float>(keys[i - 1].keyTime / FBXTimeToMs) / 1000.0f : 0.0f;

        if (time < keyTime)
        {
            if (i == 0)
            {
                return keys[i].keyValue;
            }
            else
            {
                float cValue = keys[i].keyValue;
                float pValue = keys[i - 1].keyValue;
                float normalTime = (time - prevKeyTime) / (keyTime - prevKeyTime); // 0 - 1

                return pValue * (1.0f - normalTime) + cValue * normalTime;
            }
        }
    }

    return keys[keysCount - 1].keyValue;
}