// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000

APPMAIN
{
    Red11::openConsole();
    auto window = Red11::createWindow("Window Example", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_RESIZABLE);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    auto sysInfo = new SysInfo();
    sysInfo->logIntoFile(Red11::getLogger());

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.05f, 0.05f, 0.09f));

    // Objects
    auto cube = scene->createActor<Actor>();
    auto cubeComponent = cube->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(new MaterialSimple(Color(0.2, 0.8, 0.8), Color(0.8, 0.0, 0.0)));
    cube->setPosition(Vector3(0.0f, -0.1f, -0.4f));

    auto cube2 = scene->createActor<Actor>();
    cubeComponent = cube2->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(new MaterialSimple(Color(0.4, 0.4, 0.4)));
    cube2->setPosition(Vector3(0.1f, 0.0f, -0.5f));

    auto cube3 = scene->createActor<Actor>();
    cubeComponent = cube3->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(new MaterialSimple(Color(0.8, 0.3, 0.2)));
    cube3->setPosition(Vector3(-0.05f, 0.1f, -0.55f));

    // Lights
    auto lightSun = scene->createActor<Actor>();
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(3.4f, 3.4f, 3.0f));

    auto lightOmni = scene->createActor<Actor>();
    auto lightOmniComponent = lightOmni->createComponent<ComponentLight>();
    lightOmniComponent->setupOmni(Attenuation(), Color(2.0f, 1.953f, 1.737f));
    auto lightCubeComponent = lightOmni->createComponentMesh(cubeMesh);
    lightCubeComponent->setMaterial(new MaterialSimple(Color(0.8, 0.8, 0.8), Color(0.8, 0.4, 0.2)));
    lightCubeComponent->setScale(0.12f, 0.12f, 0.12f);

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Camera
    Actor *camera = scene->createActor<Actor>();
    ComponentCamera *cameraComponent = camera->createComponent<ComponentCamera>();

    // Simple UI to show FPS
    auto fpsMeter = camera->createComponentText("FPS", font, 128);
    fpsMeter->setPosition(-0.0215f, 0.014f, -0.03f);
    fpsMeter->setScale(Vector3(0.03f, 0.03f, 0.03f));
    fpsMeter->setRotation(Vector3(CONST_PI * 0.5f, 0, 0));

    DeltaCounter deltaCounter;
    float wsDelta = 0.0f;
    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();
        fpsMeter->setText(std::string("FPS: ") + std::to_string(deltaCounter.getFPS()));

        wsDelta += delta;

        window->processWindow();

        renderer->prepareToRender();

        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());

        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        cube->rotate(Vector3(0.0f, 0.01f, 0.0f));
        cube2->rotate(Vector3(0.01f, 0.0f, 0.0f));
        cube3->rotate(Vector3(0.0f, 0.0f, 0.01f));

        lightOmni->setPosition(Vector3(sin(wsDelta) * 0.2f, cos(wsDelta) * 0.2f, -0.4f));
        camera->setPosition(Vector3(sinf(wsDelta * 0.8f) * 0.06f, 0.0f, 0.0f));

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        renderer->present();
    }

    return 0;
}