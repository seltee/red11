// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 2200
#define WINDOW_HEIGHT 1400

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

    auto window = Red11::createWindow("Textures/input Example", WINDOW_WIDTH, WINDOW_HEIGHT, false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto crateTexture = new TextureFile("Crate", "./data/crate.jpg");
    auto crateMaterial = new MaterialSimple(crateTexture);

    auto mushroomTexture = new TextureFile("Mushroom", "./data/mushroom_albedo.jpg");
    auto mushroomMaterial = new MaterialSimple(mushroomTexture);

    auto testMaterial = new MaterialSimple(Color(0.3, 0.3, 0.7));

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    auto mushroomFileData = new Data3DFile("./data/mushroom.fbx");
    mushroomFileData->load();
    auto mushroomMesh = mushroomFileData->getAsMesh();

    auto animTestFileData = new Data3DFile("./data/test.fbx");
    animTestFileData->load();
    auto animTestMesh = animTestFileData->getAsMesh();

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(1.0f, 1.0f, 1.0f));
    auto cube = scene->createActor<Actor>("Cube");
    auto cubeComponent = cube->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(crateMaterial);
    cube->setPosition(Vector3(0.0f, -0.1f, -0.5f));

    auto cube2 = scene->createActor<Actor>("Cube 2");
    cubeComponent = cube2->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(crateMaterial);
    cube2->setPosition(Vector3(0.1f, 0.0f, -0.6f));

    auto cube3 = scene->createActor<Actor>("Cube 3");
    cubeComponent = cube3->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(crateMaterial);
    cube3->setPosition(Vector3(-0.05f, 0.1f, -0.65f));

    if (mushroomMesh)
    {
        auto mushroom = scene->createActor<Actor>("Mushroom");
        auto mushroomComponent = mushroom->createComponentMesh(mushroomMesh);
        mushroomComponent->setMaterial(mushroomMaterial);
        mushroomComponent->setScale(0.0015f);
        mushroom->setPosition(Vector3(0.15f, 0.08f, -0.4f));
    }

    if (animTestMesh)
    {
        auto animtestStatic = scene->createActor<Actor>("Static Boxes");
        auto animtestComponent = animtestStatic->createComponentMesh(animTestMesh);
        animtestComponent->setMaterial(testMaterial);
        animtestComponent->setScale(0.0005f);
        animtestStatic->setPosition(Vector3(-0.25f, 0.08f, -0.6f));
    }

    auto animatedBoxes = scene->createActor<Actor>("Mushroom");
    auto animatedBoxesComponent = animatedBoxes->createComponentMeshGroup(animTestFileData->getMeshObjectList());
    animatedBoxesComponent->setMaterial(testMaterial);
    animatedBoxesComponent->setScale(0.00035f);
    auto track = animatedBoxesComponent->createAnimationTrack(animTestFileData->getAnimationsList()->at(0));
    track->loop();
    animatedBoxes->setPosition(Vector3(-0.25f, 0.28f, -0.7f));

    auto lightSun = scene->createActor<Actor>("Light");
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(0.8f, 0.8f, 0.8f));

    Camera *camera = new Camera();
    camera->setupAsPerspective(WINDOW_WIDTH, WINDOW_HEIGHT);

    Entity cameraTransform;

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
                    { ((CameraControl *)userData)->rotateX = -value; });

    InputDescriptorList rotateCameraYList;
    rotateCameraYList.addMouseInput(InputMouseType::MoveY, 1.0f);
    input->addInput(rotateCameraYList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->rotateY = -value; });

    InputDescriptorList quitButtonList;
    quitButtonList.addKeyboardInput(KeyboardCode::Escape, 1.0f);
    input->addInput(quitButtonList, window, [](InputType type, InputData *data, float value, void *userData)
                    { ((Window *)userData)->close(); });

    DeltaCounter deltaCounter;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDelta();
        window->processWindow();
        window->setMousePosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

        renderer->startRendering();
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        cube->rotate(Vector3(0.0f, 0.5f * delta, 0.0f));
        cube2->rotate(Vector3(0.5f * delta, 0.0f, 0.0f));
        cube3->rotate(Vector3(0.0f, 0.0f, 0.5f * delta));

        scene->process(delta);

        cameraTransform.rotate(Quat(Vector3(0, cameraControl.rotateX * 0.0016f, 0)));
        cameraControl.rotateX = 0.0f;
        cameraTransform.rotate(Quat(Vector3(cameraControl.rotateY * 0.0016f, 0, 0)));
        cameraControl.rotateY = 0.0f;

        auto forward = cameraTransform.getRotation() * Vector3(0, 0, 0.2f);
        cameraTransform.translate(forward * delta * cameraControl.move);
        auto side = cameraTransform.getRotation() * Vector3(0.2f, 0, 0);
        cameraTransform.translate(side * delta * cameraControl.sideMove);

        camera->updateViewMatrix(cameraTransform.getModelMatrix());
        scene->render(renderer, camera);

        renderer->endRendering();
        renderer->present();
    }

    return 0;
}