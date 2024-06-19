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

    auto window = Red11::createWindow("Bones Example", WINDOW_WIDTH, WINDOW_HEIGHT, false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto crateTexture = new TextureFile("Crate", "./data/miner_defuse.png");
    auto crateMaterial = new MaterialSimple(crateTexture);

    // Meshes
    auto boneTestFileData = new Data3DFile("./data/miner_anim_idle.fbx");
    boneTestFileData->load();

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.9f, 0.9f, 0.9f));

    auto testBox = scene->createActor<Actor>("Box");
    testBox->setPosition(Vector3(0, 0, -0.4));
    auto testBoxAnimComponent = testBox->createComponentMeshGroup(boneTestFileData->getMeshObjectList());
    testBoxAnimComponent->setMaterial(crateMaterial);
    testBoxAnimComponent->setScale(0.0032f);
    testBoxAnimComponent->setDebugBonesView(true);
    auto trackBox = testBoxAnimComponent->createAnimationTrack(boneTestFileData->getAnimationsList()->at(0));
    trackBox->loop(1.0f);

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