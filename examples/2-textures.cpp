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

    auto window = Red11::createWindow("Textures/Input Example", WINDOW_WIDTH, WINDOW_HEIGHT, false);
    window->setCursorVisibility(false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto crateTexture = new TextureFile("Crate", "./data/crate.jpg");
    auto crateNormalTexture = new TextureFile("SphereNormal", "./data/spehere_normal.png");
    auto crateMaterial = new MaterialSimple(crateTexture, crateNormalTexture);
    crateMaterial->setRoughness(0.7f);
    crateMaterial->setMetallic(0.2f);

    auto crateAlphaMaterial = new MaterialSimple(MaterialDisplay::Alpha, 0.5f, crateTexture);
    crateAlphaMaterial->setRoughness(0.7f);
    crateAlphaMaterial->setMetallic(0.2f);

    auto mushroomTexture = new TextureFile("Mushroom", "./data/mushroom_albedo.jpg");
    auto mushroomMaterial = new MaterialSimple(mushroomTexture);
    mushroomMaterial->setRoughness(0.5f);
    mushroomMaterial->setMetallic(0.0f);

    auto testMaterial = new MaterialSimple(Color(0.3, 0.3, 0.7));
    testMaterial->setRoughness(0.01f);

    auto test2Material = new MaterialSimple(nullptr, new TextureFile("BoxWithSphere", "./data/cube_n.png"));
    test2Material->setAlbedoColor(Color(0.5f, 0.5f, 0.5f));
    test2Material->setRoughness(0.8f);
    test2Material->setMetallic(0.1f);

    auto plateTexture = new TextureFile("Plate", "./data/plate.png");
    auto test3Material = new MaterialSimple(MaterialDisplay::SolidMask, 1.0f, plateTexture);
    test3Material->setRoughness(0.01f);

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    auto mushroomFileData = new Data3DFile("./data/mushroom.fbx");
    mushroomFileData->load();
    auto mushroomMesh = mushroomFileData->getAsMesh();

    auto animTestFileData = new Data3DFile("./data/test.fbx");
    animTestFileData->load();
    auto animTestMesh = animTestFileData->getAsMesh();

    auto cubeFileData = new Data3DFile("./data/test2.fbx");
    cubeFileData->load();
    auto cubeTest2Mesh = cubeFileData->getAsMesh();

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.4f, 0.4f, 0.7f));

    auto cube = scene->createActor<Actor>("Cube");
    auto cubeComponent = cube->createComponentMesh(cubeTest2Mesh);
    cubeComponent->setMaterial(test2Material);
    cubeComponent->setScale(0.0005f);
    cube->setPosition(Vector3(0.0f, -0.1f, -0.5f));

    auto cube2 = scene->createActor<Actor>("Cube 2");
    cubeComponent = cube2->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(crateAlphaMaterial);
    cube2->setPosition(Vector3(0.1f, 0.0f, -0.6f));

    auto cube3 = scene->createActor<Actor>("Cube 3");
    cubeComponent = cube3->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(test3Material);
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
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(6.7f, 5.9f, 5.0f));

    Camera camera;
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

    InputDescriptorList toggleFullscreen;
    toggleFullscreen.addKeyboardInput(KeyboardCode::KeyF, 1.0f);
    input->addInput(toggleFullscreen, window, [](InputType type, InputData *data, float value, void *userData)
                    { if (value > 0.5f) ((Window *)userData)->setFullscreen(!((Window *)userData)->isFullscreen()); });

    InputDescriptorList quitButtonList;
    quitButtonList.addKeyboardInput(KeyboardCode::Escape, 1.0f);
    input->addInput(quitButtonList, window, [](InputType type, InputData *data, float value, void *userData)
                    { ((Window *)userData)->close(); });

    DeltaCounter deltaCounter;
    float cameraRX = 0, cameraRY = 0;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDelta();
        window->processWindow();
        window->setMousePosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

        renderer->prepareToRender();
        camera.setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        cube->rotate(Vector3(0.0f, 0.5f * delta, 0.0f));
        cube2->rotate(Vector3(0.5f * delta, 0.0f, 0.0f));
        cube3->rotate(Vector3(0.0f, 0.0f, 0.5f * delta));

        scene->process(delta);

        cameraRX += cameraControl.rotateY * 0.0015f;
        cameraRY += cameraControl.rotateX * 0.0015f;
        cameraRX = glm::clamp(cameraRX, -1.2f, 1.0f);
        cameraTransform.setRotation(Vector3(cameraRX, cameraRY, 0));

        auto forward = cameraTransform.getRotation() * Vector3(0, 0, 0.2f);
        cameraTransform.translate(forward * delta * cameraControl.move);
        auto side = cameraTransform.getRotation() * Vector3(0.2f, 0, 0);
        cameraTransform.translate(side * delta * cameraControl.sideMove);

        camera.updateViewMatrix(cameraTransform.getModelMatrix());
        scene->render(renderer, &camera);

        renderer->present();
    }

    return 0;
}