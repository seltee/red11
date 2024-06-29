// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1200

APPMAIN
{
    Red11::openConsole();
    auto window = Red11::createWindow("Window Example", WINDOW_WIDTH, WINDOW_HEIGHT, false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.05f, 0.05f, 0.09f));

    auto cube = scene->createActor<Actor>("Cube");
    auto cubeComponent = cube->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(new MaterialSimple(Color(0.2, 0.8, 0.8), Color(0.8, 0.0, 0.0)));
    cube->setPosition(Vector3(0.0f, -0.1f, -0.4f));

    auto cube2 = scene->createActor<Actor>("Cube 2");
    cubeComponent = cube2->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(new MaterialSimple(Color(0.4, 0.4, 0.4)));
    cube2->setPosition(Vector3(0.1f, 0.0f, -0.5f));

    auto cube3 = scene->createActor<Actor>("Cube 3");
    cubeComponent = cube3->createComponentMesh(cubeMesh);
    cubeComponent->setMaterial(new MaterialSimple(Color(0.8, 0.3, 0.2)));
    cube3->setPosition(Vector3(-0.05f, 0.1f, -0.55f));

    auto lightSun = scene->createActor<Actor>("Light");
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(3.4f, 3.4f, 3.0f));

    auto lightOmni = scene->createActor<Actor>("Omni");
    auto lightOmniComponent = lightOmni->createComponent<ComponentLight>();
    lightOmniComponent->setupOmni(Attenuation(), Color(2.0f, 1.953f, 1.737f));
    auto lightCubeComponent = lightOmni->createComponentMesh(cubeMesh);
    lightCubeComponent->setMaterial(new MaterialSimple(Color(0.8, 0.8, 0.8), Color(0.8, 0.4, 0.2)));
    lightCubeComponent->setScale(0.12f, 0.12f, 0.12f);

    Camera *camera = new Camera();
    Entity cameraTransform;

    camera->setupAsPerspective(WINDOW_WIDTH, WINDOW_HEIGHT);

    DeltaCounter deltaCounter;
    float wsDelta = 0.0f;
    while (!window->isCloseRequested())
    {
        window->processWindow();
        wsDelta += deltaCounter.getDelta();

        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        cube->rotate(Vector3(0.0f, 0.01f, 0.0f));
        cube2->rotate(Vector3(0.01f, 0.0f, 0.0f));
        cube3->rotate(Vector3(0.0f, 0.0f, 0.01f));

        lightOmni->setPosition(Vector3(sin(wsDelta) * 0.2f, cos(wsDelta) * 0.2f, -0.4f));

        scene->process(0.01f);

        cameraTransform.setPosition(Vector3(sinf(wsDelta * 0.8f) * 0.06f, 0.0f, 0.0f));
        camera->updateViewMatrix(cameraTransform.getModelMatrix());
        scene->render(renderer, camera);

        renderer->present();
    }

    return 0;
}