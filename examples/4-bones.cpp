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
    bool lowerShadowQuality;
    bool higherShadowQuality;
};

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Bones Example", WINDOW_WIDTH, WINDOW_HEIGHT, false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials

    auto monTexture = new TextureFile("MonsterDefuse", "./data/miner_defuse.png");
    auto monMaterial = new MaterialSimple(monTexture);
    monMaterial->setRoughness(0.25f);

    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto sphereNormalTexture = new TextureFile("SphereNormal", "./data/spehere_normal.png");
    auto concreteMaterial = new MaterialSimple(concreteTexture, sphereNormalTexture);
    // auto concreteMaterial = new MaterialSimple(concreteTexture);

    // Meshes
    auto minerIdleFileData = new Data3DFile("./data/miner_anim_idle.fbx");
    minerIdleFileData->load();

    auto minerDeathFileData = new Data3DFile("./data/miner_death.fbx");
    minerDeathFileData->load();

    auto minerShootingFileData = new Data3DFile("./data/miner_anim_shooting.fbx");
    minerShootingFileData->load();

    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.4f, 0.4f, 0.6f));

    int rows = 6;
    for (int i = 0; i < rows * rows; i++)
    {
        int ix = i % rows;
        int iy = i / rows;

        float xPos = (float)(rows / 2 - ix) * 0.5;
        float yPos = (float)(iy) * -0.4 - 0.5;

        auto anim = minerIdleFileData;
        if ((ix % 3) == 2)
            anim = minerShootingFileData;

        auto testMon = scene->createActor<Actor>("Mon");
        testMon->setPosition(Vector3(xPos, 0, yPos));
        auto testMonAnimComponent = testMon->createComponentMeshGroup(anim->getMeshObjectList());
        testMonAnimComponent->setMaterial(monMaterial);
        testMonAnimComponent->setScale(0.0018f);
        auto trackBox = testMonAnimComponent->createAnimationTrack(anim->getAnimationsList()->at(0));
        trackBox->loop(0.6f);
    }

    for (int iy = 0; iy < 9; iy++)
    {
        for (int ix = 0; ix < 9; ix++)
        {
            auto floorCube = scene->createActor<Actor>("FloorCell");
            auto floorCubeComponent = floorCube->createComponentMesh(cubeMesh);
            floorCubeComponent->setMaterial(concreteMaterial);
            floorCubeComponent->setPosition(Vector3((float)(ix - 4) * 0.8f, 0.0f, (float)(iy - 5) * 0.8f));
            floorCubeComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
        }
    }

    auto lightSun = scene->createActor<Actor>("Light");
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    int shadowQuality = (int)LightShadowQuality::Low;
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(6.8f, 5.8f, 5.2f), true, (LightShadowQuality)shadowQuality);

    // light shadow presenter
    auto lightShadowPresenter = scene->createActor<Actor>("LightShadowPresenter");
    auto lightShadowComponent = lightShadowPresenter->createComponentMesh(cubeMesh);
    auto lightShadowMaterial = new MaterialSimple(lightSunComponent->getLight()->getShadowTexture(0));
    lightShadowMaterial->setAlbedoColor(Color(1, 0, 0));
    lightShadowComponent->setMaterial(lightShadowMaterial);
    lightShadowComponent->setPosition(0.0f, 0.9f, -2.5f);
    lightShadowComponent->setScale(Vector3(4.0f, 4.0f, 4.0f));

    Camera *camera = new Camera();
    camera->setupAsPerspective(WINDOW_WIDTH, WINDOW_HEIGHT);

    Entity cameraTransform;
    cameraTransform.setPosition(0, 0.2, 0);

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

    InputDescriptorList lowerShadowQualityList;
    lowerShadowQualityList.addKeyboardInput(KeyboardCode::KeyQ, 1.0f);
    input->addInput(lowerShadowQualityList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->lowerShadowQuality = value > 0.5; });

    InputDescriptorList higherShadowQualityList;
    higherShadowQualityList.addKeyboardInput(KeyboardCode::KeyE, 1.0f);
    input->addInput(higherShadowQualityList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->higherShadowQuality = value > 0.5; });

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

        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        scene->process(delta);

        cameraTransform.rotate(Quat(Vector3(0, cameraControl.rotateX * 0.0016f, 0)));
        cameraControl.rotateX = 0.0f;
        cameraTransform.rotate(Quat(Vector3(cameraControl.rotateY * 0.0016f, 0, 0)));
        cameraControl.rotateY = 0.0f;

        auto forward = cameraTransform.getRotation() * Vector3(0, 0, 0.4f);
        cameraTransform.translate(forward * delta * cameraControl.move);
        auto side = cameraTransform.getRotation() * Vector3(0.4f, 0, 0);
        cameraTransform.translate(side * delta * cameraControl.sideMove);

        if (cameraControl.lowerShadowQuality)
        {
            cameraControl.lowerShadowQuality = false;
            if (shadowQuality > 0)
            {
                shadowQuality--;
                lightSunComponent->setShadowState(true, (LightShadowQuality)shadowQuality);
                lightShadowMaterial->setAlbedoTexture(lightSunComponent->getLight()->getShadowTexture(0));
            }
        }

        if (cameraControl.higherShadowQuality)
        {
            cameraControl.higherShadowQuality = false;
            if (shadowQuality < (int)LightShadowQuality::AmountOfValues - 1)
            {
                shadowQuality++;
                lightSunComponent->setShadowState(true, (LightShadowQuality)shadowQuality);
                lightShadowMaterial->setAlbedoTexture(lightSunComponent->getLight()->getShadowTexture(0));
            }
        }

        camera->updateViewMatrix(cameraTransform.getModelMatrix());
        scene->render(renderer, camera);

        renderer->present();
    }

    return 0;
}