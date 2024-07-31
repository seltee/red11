// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

struct PlayerControl
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
    window->setCursorVisibility(false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Materials and textures
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);

    auto crateTexture = new TextureFile("Concrete", "./data/crate.jpg");
    auto crateMaterial = new MaterialSimple(crateTexture);

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    auto manFileData = new Data3DFile("./data/man.fbx");
    auto manWalkFileData = new Data3DFile("./data/man_walk.fbx", false);
    auto manWalkBackwardsFileData = new Data3DFile("./data/man_walk_backwards.fbx", false);
    auto manStraifLeftFileData = new Data3DFile("./data/man_straif_left.fbx", false);
    auto manStraifRightFileData = new Data3DFile("./data/man_straif_right.fbx", false);

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.2f, 0.2f, 0.2f));

    for (int iy = 0; iy < 5; iy++)
    {
        for (int ix = 0; ix < 5; ix++)
        {
            auto floorCube = scene->createActor<Actor>("FloorCell");
            auto floorCubeComponent = floorCube->createComponentMesh(cubeMesh);
            floorCubeComponent->setMaterial(concreteMaterial);
            floorCubeComponent->setPosition(Vector3((float)(ix - 2) * 0.8f, -0.1f, (float)(iy - 2) * 0.8f));
            floorCubeComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
        }
    }

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Player actor
    auto player = scene->createActor<Actor>("Player");
    auto playerAnimComponent = player->createComponentMeshGroup(manFileData->getMeshObjectList());
    playerAnimComponent->setMaterial(crateMaterial);
    playerAnimComponent->setScale(0.00032f);
    auto trackIdle = playerAnimComponent->createAnimationTrack(manFileData->getAnimationsList()->at(0));
    trackIdle->loop(0.8f);
    auto trackWalk = playerAnimComponent->createAnimationTrack(manWalkFileData->getAnimationsList()->at(0));
    trackWalk->loop(2.0f);
    auto trackWalkBackwards = playerAnimComponent->createAnimationTrack(manWalkBackwardsFileData->getAnimationsList()->at(0));
    trackWalkBackwards->loop(2.0f);
    auto trackStraifLeft = playerAnimComponent->createAnimationTrack(manStraifLeftFileData->getAnimationsList()->at(0));
    trackStraifLeft->loop(2.0f, 0.5f);
    auto trackStraifRight = playerAnimComponent->createAnimationTrack(manStraifRightFileData->getAnimationsList()->at(0));
    trackStraifRight->loop(2.0f, 0.5f);

    // camera
    auto cameraRoot = player->createComponent<Component>();
    auto cameraComponent = player->createComponent<ComponentCamera>();
    cameraComponent->setPosition(Vector3(0, 0.16f, 0.4f));
    cameraComponent->setParent(cameraRoot);

    // Simple UI to show FPS
    auto fpsMeter = player->createComponentText("FPS", font, 128);
    fpsMeter->setParent(cameraComponent);
    fpsMeter->setPosition(-0.022f, 0.012f, -0.03);
    fpsMeter->setScale(Vector3(0.03f, 0.03f, 0.03f));
    fpsMeter->setRotation(Vector3(CONST_PI * 0.5f, 0, 0));

    // Light
    auto lightSun = scene->createActor<Actor>("Light");
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(5.8f, 5.8f, 5.8f));

    // Controls
    PlayerControl playerControl;
    memset(&playerControl, 0, sizeof(PlayerControl));

    auto input = Red11::getGlobalInputProvider();
    InputDescriptorList forwardList;
    forwardList.addKeyboardInput(KeyboardCode::Up, 1.0f);
    forwardList.addKeyboardInput(KeyboardCode::Down, -1.0f);
    forwardList.addKeyboardInput(KeyboardCode::KeyW, 1.0f);
    forwardList.addKeyboardInput(KeyboardCode::KeyS, -1.0f);
    input->addInput(forwardList, &playerControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((PlayerControl *)userData)->move = -value; });

    InputDescriptorList sideList;
    sideList.addKeyboardInput(KeyboardCode::Left, 1.0f);
    sideList.addKeyboardInput(KeyboardCode::Right, -1.0f);
    sideList.addKeyboardInput(KeyboardCode::KeyA, 1.0f);
    sideList.addKeyboardInput(KeyboardCode::KeyD, -1.0f);
    input->addInput(sideList, &playerControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((PlayerControl *)userData)->sideMove = -value; });

    InputDescriptorList rotateCameraXList;
    rotateCameraXList.addMouseInput(InputMouseType::MoveX, 1.0f);
    input->addInput(rotateCameraXList, &playerControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((PlayerControl *)userData)->rotateX = -value; });

    InputDescriptorList rotateCameraYList;
    rotateCameraYList.addMouseInput(InputMouseType::MoveY, 1.0f);
    input->addInput(rotateCameraYList, &playerControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((PlayerControl *)userData)->rotateY = -value; });

    InputDescriptorList toggleFullscreen;
    toggleFullscreen.addKeyboardInput(KeyboardCode::KeyF, 1.0f);
    input->addInput(toggleFullscreen, window, [](InputType type, InputData *data, float value, void *userData)
                    { if (value > 0.5f) ((Window *)userData)->setFullscreen(!((Window *)userData)->isFullscreen()); });

    InputDescriptorList quitButtonList;
    quitButtonList.addKeyboardInput(KeyboardCode::Escape, 1.0f);
    input->addInput(quitButtonList, window, [](InputType type, InputData *data, float value, void *userData)
                    { ((Window *)userData)->close(); });

    DeltaCounter deltaCounter;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();
        fpsMeter->setText(std::string("FPS: ") + std::to_string(deltaCounter.getFPS()));

        window->processWindow();
        window->setMousePosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

        renderer->prepareToRender();
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        player->rotate(Quat(Vector3(0, playerControl.rotateX * 0.0016f, 0)));
        playerControl.rotateX = 0.0f;

        cameraRoot->rotate(Quat(Vector3(playerControl.rotateY * 0.0016f, 0, 0)));
        playerControl.rotateY = 0.0f;

        // Move
        const float speed = 0.3f;
        auto forward = glm::normalize(player->getRotation() * Vector3(0, 0, 1.0f));
        auto side = glm::normalize(player->getRotation() * Vector3(0.6f, 0, 0));

        auto moveDirection = (forward * playerControl.move) + (side * playerControl.sideMove);
        auto moveTranslate = Vector3(0.0f);

        if (glm::length2(moveDirection) > 0.5f)
        {
            Vector3 nMoveDirection = glm::normalize(moveDirection);
            moveTranslate = nMoveDirection * speed * delta;
            float angleRadians = glm::angle(nMoveDirection, -forward);

            float forwardAmount = 1.0f - (angleRadians / (CONST_PI * 0.5f));
            float sideAmount = 1.0f - fabs(forwardAmount);

            trackIdle->setWeight(0.0f);

            trackWalk->setWeight(forwardAmount > 0.0f ? fabsf(forwardAmount) : 0.0f);
            trackWalkBackwards->setWeight(forwardAmount < 0.0f ? fabsf(forwardAmount) : 0.0f);

            trackStraifLeft->setWeight(playerControl.sideMove < 0.0f ? sideAmount : 0.0f);
            trackStraifRight->setWeight(playerControl.sideMove > 0.0f ? sideAmount : 0.0f);
        }
        else
        {
            trackIdle->setWeight(1.0f);
            trackWalk->setWeight(0.0f);
            trackWalkBackwards->setWeight(0.0f);
            trackStraifLeft->setWeight(0.0f);
            trackStraifRight->setWeight(0.0f);
        }

        player->translate(moveTranslate);
        player->setPositionY(-0.1f);

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        renderer->present();
    }

    return 0;
}