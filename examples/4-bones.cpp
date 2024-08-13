// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

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

    auto window = Red11::createWindow("Bones Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    window->setCursorVisibility(false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto monTexture = new TextureFile("MonsterDefuse", "./data/miner_defuse.png");
    auto monMaterial = new MaterialSimple(monTexture);
    monMaterial->setRoughness(0.7f);

    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto sphereNormalTexture = new TextureFile("SphereNormal", "./data/spehere_normal.png");
    auto concreteMaterial = new MaterialSimple(concreteTexture, sphereNormalTexture);
    concreteMaterial->setRoughness(0.52f);

    auto hdr = new TextureFileHDR("HDR", "./data/meadow.hdr", 1.8f, 2.9f);
    auto radiance = hdr->getRadianceTexture();

    // Meshes
    auto minerIdleFileData = new Data3DFile("./data/miner_anim_idle.fbx");
    minerIdleFileData->load();

    auto minerDeathFileData = new Data3DFile("./data/miner_death.fbx");
    minerDeathFileData->load();

    auto minerShootingFileData = new Data3DFile("./data/miner_anim_shooting.fbx");
    minerShootingFileData->load();

    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.10f, 0.11f, 0.12f));

    // Monsters
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

        auto testMon = scene->createActor<Actor>();
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
            auto floorCube = scene->createActor<Actor>();
            auto floorCubeComponent = floorCube->createComponentMesh(cubeMesh);
            floorCubeComponent->setMaterial(concreteMaterial);
            floorCubeComponent->setPosition(Vector3((float)(ix - 4) * 0.8f, 0.0f, (float)(iy - 5) * 0.8f));
            floorCubeComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
        }
    }

    auto lightSun = scene->createActor<Actor>();
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    int shadowQuality = (int)LightShadowQuality::Low;
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(2.2f, 1.5f, 1.4f), true, (LightShadowQuality)shadowQuality);

    // light shadow presenter
    auto lightShadowPresenter = scene->createActor<Actor>();
    auto lightShadowComponent = lightShadowPresenter->createComponentMesh(cubeMesh);
    auto lightShadowMaterial = new MaterialSimple(lightSunComponent->getLight()->getShadowTexture(0));
    lightShadowMaterial->setAlbedoColor(Color(1.2f, 0, 0));
    lightShadowComponent->setMaterial(lightShadowMaterial);
    lightShadowComponent->setPosition(0.0f, 0.9f, -2.5f);
    lightShadowComponent->setScale(Vector3(4.0f, 4.0f, 4.0f));

    // rotating lights
    const int omniLightsAmount = 10;
    Actor *omniActors[omniLightsAmount];
    for (int i = 0; i < omniLightsAmount; i++)
    {
        omniActors[i] = scene->createActor<Actor>(std::string("LightOmni_") + std::to_string(i));
        Color color = Color(randf(20.0f, 60.0f), randf(20.0f, 60.0f), randf(20.0f, 60.0f));
        auto omniComponentBox = omniActors[i]->createComponentMesh(cubeMesh);
        auto omniMaterial = new MaterialSimple(Color(0, 0, 0), color);
        omniComponentBox->setMaterial(omniMaterial);
        omniComponentBox->setScale(0.3f);
        auto lightOmniComponent = omniActors[i]->createComponent<ComponentLight>();
        lightOmniComponent->setupOmni(Attenuation(16.0f, 4.0f, 40.0f), color);
    }

    // Camera
    Actor *camera = scene->createActor<Actor>();
    ComponentCamera *cameraComponent = camera->createComponent<ComponentCamera>();
    camera->setPosition(0, 0.2, 0);

    CameraControl cameraControl;
    memset(&cameraControl, 0, sizeof(CameraControl));

    // Simple UI to show FPS
    auto fpsMeter = camera->createComponentText("FPS", font, 128);
    fpsMeter->setPosition(-0.022f, 0.012f, -0.03f);
    fpsMeter->setScale(Vector3(0.03f, 0.03f, 0.03f));
    fpsMeter->setRotation(Vector3(CONST_PI * 0.5f, 0, 0));

    // Input
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

    InputDescriptorList lowerShadowQualityList;
    lowerShadowQualityList.addKeyboardInput(KeyboardCode::KeyQ, 1.0f);
    input->addInput(lowerShadowQualityList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->lowerShadowQuality = value > 0.5; });

    InputDescriptorList higherShadowQualityList;
    higherShadowQualityList.addKeyboardInput(KeyboardCode::KeyE, 1.0f);
    input->addInput(higherShadowQualityList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((CameraControl *)userData)->higherShadowQuality = value > 0.5; });

    InputDescriptorList toggleFullscreen;
    toggleFullscreen.addKeyboardInput(KeyboardCode::KeyF, 1.0f);
    input->addInput(toggleFullscreen, window, [](InputType type, InputData *data, float value, void *userData)
                    { if (value > 0.5f) ((Window *)userData)->setFullscreen(!((Window *)userData)->isFullscreen()); });

    InputDescriptorList quitButtonList;
    quitButtonList.addKeyboardInput(KeyboardCode::Escape, 1.0f);
    input->addInput(quitButtonList, window, [](InputType type, InputData *data, float value, void *userData)
                    { ((Window *)userData)->close(); });

    DeltaCounter deltaCounter;
    float boxRotateCounter = 0.0f;
    float cameraRX = 0, cameraRY = 0;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();
        fpsMeter->setText(std::string("FPS: ") + std::to_string(deltaCounter.getFPS()));
        boxRotateCounter += delta * 0.06f;

        window->processWindow();
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);

        renderer->prepareToRender(hdr, radiance);
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));
        renderer->renderCubeMap(cameraComponent->getCamera(), cameraComponent, hdr);

        cameraRX += cameraControl.rotateY * 0.0015f;
        cameraRY += cameraControl.rotateX * 0.0015f;
        cameraRX = glm::clamp(cameraRX, -1.2f, 1.0f);
        camera->setRotation(Vector3(cameraRX, cameraRY, 0));

        auto forward = camera->getRotation() * Vector3(0, 0, 0.4f);
        camera->translate(forward * delta * cameraControl.move);
        auto side = camera->getRotation() * Vector3(0.4f, 0, 0);
        camera->translate(side * delta * cameraControl.sideMove);

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

        float omniStep = CONST_PI2 / ((float)omniLightsAmount);
        for (int i = 0; i < omniLightsAmount; i++)
        {
            float angle = ((float)i) * omniStep + boxRotateCounter;
            const float distance = 2.6f;
            omniActors[i]->setPosition(Vector3(sinf(angle) * distance, 0.16f, cosf(angle) * distance));
        }

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        renderer->present();
    }

    return 0;
}