// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

// #define WINDOW_WIDTH 680 * 2
// #define WINDOW_HEIGHT 1080 * 2
#define WINDOW_WIDTH 960 * 2
#define WINDOW_HEIGHT 960 * 2

struct CameraControl
{
    float move;
    float sideMove;
    float rotateX;
    float rotateY;
};

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Demo 1", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto floorTextureAlbedo = new TextureFile("FloorTextureAlbedo", "./data/demo/floor_albedo.jpg");
    auto floorTextureNormal = new TextureFile("FloorTextureNormal", "./data/demo/floor_normal.jpg");
    auto floorTextureRoughness = new TextureFile("FloorTextureRoughness", "./data/demo/floor_roughness.jpg");
    auto floorMaterial = new MaterialSimple(floorTextureAlbedo, floorTextureNormal, nullptr, nullptr, floorTextureRoughness);
    floorMaterial->setRoughness(0.55f);

    auto wallTextureAlbedo = new TextureFile("Wall", "./data/demo/wall_albedo.jpg");
    auto wallMaterial = new MaterialSimple(wallTextureAlbedo);
    wallMaterial->setRoughness(0.65f);

    auto wallAddAlbedo = new TextureFile("WallAddTextureAlbedo", "./data/demo/wall_add_albedo.jpg");
    auto wallAddNormal = new TextureFile("WallAddTextureNormal", "./data/demo/wall_add_normal.jpg");
    auto wallAddRoughness = new TextureFile("WallAddTextureRoughness", "./data/demo/wall_add_roughness.jpg");
    auto wallAddMaterial = new MaterialSimple(wallAddAlbedo, wallAddNormal, nullptr, nullptr, wallAddRoughness);
    wallAddMaterial->setRoughness(0.7f);

    auto ceilingAlbedo = new TextureFile("CeilingTextureAlbedo", "./data/demo/ceiling_albedo.jpg");
    auto ceilingMaterial = new MaterialSimple(ceilingAlbedo);
    ceilingMaterial->setRoughness(0.8f);

    auto doorAlbedo = new TextureFile("WallAddTextureAlbedo", "./data/demo/door_albedo.jpg");
    auto doorNormal = new TextureFile("WallAddTextureNormal", "./data/demo/door_normal.jpg");
    auto doorMetal = new TextureFile("WallAddTextureRoughness", "./data/demo/door_metal.jpg");
    auto doorRoughness = new TextureFile("WallAddTextureRoughness", "./data/demo/door_roughness.jpg");
    auto doorMaterial = new MaterialSimple(doorAlbedo, doorNormal, nullptr, doorMetal, doorRoughness);
    doorMaterial->setRoughness(0.7f);

    auto lampAlbedo = new TextureFile("WallAddTextureAlbedo", "./data/demo/lamp_albedo.jpg");
    auto lampEmission = new TextureFile("WallAddTextureEmission", "./data/demo/lamp_emission.jpg");
    auto lampMaterial = new MaterialSimple(lampAlbedo, nullptr, lampEmission);
    lampMaterial->setRoughness(0.5f);

    auto paintBoardAlbedo = new TextureFile("PaintBoardTextureAlbedo", "./data/demo/paint_board_albedo.jpg");
    auto paintBoardRoughness = new TextureFile("PaintBoardTextureRoughness", "./data/demo/paint_board_roughness.jpg");
    auto paintBoardMaterial = new MaterialSimple(paintBoardAlbedo, nullptr, nullptr, nullptr, paintBoardRoughness);
    paintBoardMaterial->setRoughness(0.8f);

    auto flashlight = new TextureFile("FlashlightTexture", "./data/demo/flashlight.jpg");

    // Room Meshes
    auto roomFloorFileData = new Data3DFile("./data/demo/room_floor.fbx");
    auto roomWallsFileData = new Data3DFile("./data/demo/room_walls.fbx");
    auto roomWallsAddFileData = new Data3DFile("./data/demo/room_walls_add.fbx");
    auto ceilingFileData = new Data3DFile("./data/demo/room_ceiling.fbx");
    auto doorFileData = new Data3DFile("./data/demo/room_door.fbx");
    auto lampFileData = new Data3DFile("./data/demo/lamp.fbx");
    auto paintBoardFileData = new Data3DFile("./data/demo/paint_board.fbx");

    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.15f, 0.14f, 0.1f));

    const float scale = 0.002f;
    const float scaleNormal = scale / 0.01f;

    auto room = scene->createActor<Actor>("Room");
    auto floorComponent = room->createComponentMesh(roomFloorFileData->getAsMesh());
    floorComponent->setMaterial(floorMaterial);
    floorComponent->setScale(scale);

    auto wallsComponent = room->createComponentMesh(roomWallsFileData->getAsMesh());
    wallsComponent->setMaterial(wallMaterial);
    wallsComponent->setScale(scale);

    auto wallsAddComponent = room->createComponentMesh(roomWallsAddFileData->getAsMesh());
    wallsAddComponent->setMaterial(wallAddMaterial);
    wallsAddComponent->setScale(scale);

    auto ceilingComponent = room->createComponentMesh(ceilingFileData->getAsMesh());
    ceilingComponent->setMaterial(ceilingMaterial);
    ceilingComponent->setScale(scale);

    auto doorComponent = room->createComponentMesh(doorFileData->getAsMesh());
    doorComponent->setMaterial(doorMaterial);
    doorComponent->setScale(scale);

    auto paintBoardComponent = room->createComponentMesh(paintBoardFileData->getAsMesh());
    paintBoardComponent->setMaterial(paintBoardMaterial);
    paintBoardComponent->setScale(scale);

    std::vector<Actor *> lightActors;
    Color lightColor = Color(46.0f, 38.0f, 34.0f);
    auto generateSpotLight = [&](const Vector3 &center, bool shadows)
    {
        auto light = scene->createActor<Actor>("Light");
        light->setPosition(center);

        auto lightComponent = light->createComponentMesh(lampFileData->getAsMesh());
        lightComponent->setMaterial(lampMaterial);
        lightComponent->setScale(scale);

        auto lightSpot = light->createComponent<ComponentLight>();
        lightSpot->setupSpot(Vector3(0, -1.0f, 0), Attenuation(4.0f, 1.0f, 6.0f), glm::radians(58.0f), glm::radians(24.0f), lightColor, shadows, LightShadowQuality::Ultra);
        lightSpot->setPosition(Vector3(0, -0.069f, 0));
        lightSpot->getLight()->setShadowMaskTexture(flashlight);

        lightActors.push_back(light);

        return lightSpot;
    };

    auto lightSpot = generateSpotLight(Vector3(-1.25f, 3.0f, 1.36f) * scaleNormal, true);
    generateSpotLight(Vector3(-1.25f, 3.0f, 8.36f) * scaleNormal, true);
    generateSpotLight(Vector3(-8.25f, 3.0f, 1.36f) * scaleNormal, true);

    // Light shadow presenter
    auto lightShadowPresenter = scene->createActor<Actor>("LightShadowPresenter");
    auto lightShadowComponent = lightShadowPresenter->createComponentMesh(cubeMesh);
    auto lightShadowMaterial = new MaterialSimple(lightSpot->getLight()->getShadowTexture(0));
    lightShadowMaterial->setAlbedoColor(Color(1, 0, 0));
    lightShadowComponent->setMaterial(lightShadowMaterial);
    lightShadowComponent->setPosition(Vector3(-1.25f, 0.3f, 2.4f) * scaleNormal);
    lightShadowComponent->setScale(Vector3(1.2f, 1.2f, 1.2f));

    auto lightShadowCasterComponent = lightShadowPresenter->createComponentMesh(cubeMesh);
    lightShadowCasterComponent->setMaterial(lightShadowMaterial);
    lightShadowCasterComponent->setScale(Vector3(0.2f, 0.2f, 0.2f));

    // Camera and controls
    Camera camera;
    Entity cameraTransform;
    cameraTransform.setPosition(-0.6, 0.6, 0.6);

    CameraControl cameraControl;
    memset(&cameraControl, 0, sizeof(CameraControl));

    auto input = Red11::getGlobalInputProvider();
    InputDescriptorList forwardList;
    forwardList.addKeyboardInput(KeyboardCode::Up, 1.0f);
    forwardList.addKeyboardInput(KeyboardCode::Down, -1.0f);
    forwardList.addKeyboardInput(KeyboardCode::KeyW, 1.0f);
    forwardList.addKeyboardInput(KeyboardCode::KeyS, -1.0f);
    input->addInput(forwardList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->move = -value; });

    InputDescriptorList sideList;
    sideList.addKeyboardInput(KeyboardCode::Left, 1.0f);
    sideList.addKeyboardInput(KeyboardCode::Right, -1.0f);
    sideList.addKeyboardInput(KeyboardCode::KeyA, 1.0f);
    sideList.addKeyboardInput(KeyboardCode::KeyD, -1.0f);
    input->addInput(sideList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->sideMove = -value; });

    InputDescriptorList rotateCameraXList;
    rotateCameraXList.addMouseInput(InputMouseType::MoveX, 1.0f);
    input->addInput(rotateCameraXList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->rotateX = glm::clamp(-value, -64.0f, 64.0f); });

    InputDescriptorList rotateCameraYList;
    rotateCameraYList.addMouseInput(InputMouseType::MoveY, 1.0f);
    input->addInput(rotateCameraYList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->rotateY = glm::clamp(-value, -64.0f, 64.0f); });

    InputDescriptorList quitButtonList;
    quitButtonList.addKeyboardInput(KeyboardCode::Escape, 1.0f);
    input->addInput(quitButtonList, window, [](InputType type, InputData *data, float value, void *userData)
                    { ((Window *)userData)->close(); });

    InputDescriptorList toggleFullscreen;
    toggleFullscreen.addKeyboardInput(KeyboardCode::KeyF, 1.0f);
    input->addInput(toggleFullscreen, window, [](InputType type, InputData *data, float value, void *userData)
                    { if (value > 0.5f) ((Window *)userData)->setFullscreen(!((Window *)userData)->isFullscreen()); });

    DeltaCounter deltaCounter;
    float cameraRX = 0, cameraRY = 0;
    float accDelta = 0.0f;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDelta();
        accDelta += delta;

        float add = 0.0f;
        for (auto &light : lightActors)
        {
            float rCos = cos(accDelta * 1.75f + add);
            light->setRotation(Vector3(rCos * 0.05f, 0, 0));
            add += 1.0f;
        }

        window->processWindow();
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);

        renderer->prepareToRender();
        camera.setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        scene->process(delta);

        cameraRX += cameraControl.rotateY * 0.0015f;
        cameraRY += cameraControl.rotateX * 0.0015f;
        cameraRX = glm::clamp(cameraRX, -1.2f, 1.0f);
        cameraTransform.setRotation(Vector3(cameraRX, cameraRY, 0));

        auto forward = cameraTransform.getRotation() * Vector3(0, 0, 1.2f);
        cameraTransform.translate(forward * delta * cameraControl.move);
        auto side = cameraTransform.getRotation() * Vector3(1.2f, 0, 0);
        cameraTransform.translate(side * delta * cameraControl.sideMove);

        lightShadowCasterComponent->setPosition(cameraTransform.getPosition());

        camera.updateViewMatrix(cameraTransform.getModelMatrix());
        scene->render(renderer, &camera);

        renderer->present();
    }

    return 0;
}