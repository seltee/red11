// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

struct InputControl
{
    float move;
    float sideMove;
    float rotateX;
    float rotateY;
};

enum class UIButtonID
{
    Help,
    AddBall,
    File,
    Modal
};
class UIUserData
{
public:
    UIUserData(UIButtonID button)
    {
        this->button = button;
    }
    UIButtonID button;
};

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Resource Managment", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    auto videoModes = window->getVideoModesList(60, true);
    Red11::getLogger()->logConsole("Available resolutions:");
    for (auto &mode : videoModes)
    {
        Red11::getLogger()->logConsole("%ix%i", mode.width, mode.height);
    }

    auto refreshRates = window->getRefreshRatesList();
    Red11::getLogger()->logConsole("Refresh Rates:");
    for (auto &rate : refreshRates)
    {
        Red11::getLogger()->logConsole("%ihz", rate);
    }

    if (window->isResolutionAvailable(3840, 2160, true))
        Red11::getLogger()->logConsole("4k is available");
    else
        Red11::getLogger()->logConsole("4k is not available");

    unsigned long long llu = 198237198289993278;
    Red11::getLogger()->logConsole("llu number %llu", llu);

    // Textures & Materials
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);
    concreteMaterial->setRoughness(0.6f);

    auto crateTexture = new TextureFile("Crate", "./data/crate.jpg");
    auto crateMaterial = new MaterialSimple(crateTexture);
    crateMaterial->setRoughness(0.7f);
    crateMaterial->setMetallic(0.2f);

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);
    auto cubeMeshBig = Red11::getMeshBuilder()->createCube(0.25f);

    Scene *scene = nullptr;
    Actor *camera = nullptr;
    ComponentCamera *cameraComponent = nullptr;
    Actor *objectContainer = nullptr;
    ComponentMesh *boxComponent = nullptr;
    for (int i = 0; i < 100000; i++)
    {
        if (scene)
            scene->destroy();

        // Scene
        scene = Red11::createScene();
        scene->setAmbientLight(Color(0.4f, 0.4f, 0.6f));

        // Objects
        objectContainer = scene->createActor<Actor>();

        boxComponent = objectContainer->createComponentMesh(cubeMeshBig);
        boxComponent->setMaterial(crateMaterial);
        boxComponent->setPosition(randf(-3.0f, 3.0f), 0.8f, randf(-3.0f, 3.0f));

        // Floor
        for (int iy = 0; iy < 9; iy++)
        {
            for (int ix = 0; ix < 9; ix++)
            {
                auto floorCubeComponent = objectContainer->createComponentMesh(cubeMesh);
                floorCubeComponent->setMaterial(concreteMaterial);
                floorCubeComponent->setPosition(Vector3((float)(ix - 4) * 0.8f, 0.0f, (float)(iy - 4) * 0.8f));
                floorCubeComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
            }
        }

        // Light
        auto lightSun = scene->createActor<Actor>();
        auto lightSunComponent = lightSun->createComponent<ComponentLight>();
        lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(3.8f, 3.4f, 3.2f), true, LightShadowQuality::Maximum);

        // Camera
        camera = scene->createActor<Actor>();
        cameraComponent = camera->createComponent<ComponentCamera>();
        camera->setPosition(0, 0.2, 0);
    }

    InputControl inputControl;
    memset(&inputControl, 0, sizeof(InputControl));

    // Input
    auto input = Red11::getGlobalInputProvider();
    InputDescriptorList forwardList;
    forwardList.addKeyboardInput(KeyboardCode::Up, 1.0f);
    forwardList.addKeyboardInput(KeyboardCode::Down, -1.0f);
    forwardList.addKeyboardInput(KeyboardCode::KeyW, 1.0f);
    forwardList.addKeyboardInput(KeyboardCode::KeyS, -1.0f);
    input->addInput(forwardList, &inputControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((InputControl *)userData)->move = -value; });

    InputDescriptorList sideList;
    sideList.addKeyboardInput(KeyboardCode::Left, 1.0f);
    sideList.addKeyboardInput(KeyboardCode::Right, -1.0f);
    sideList.addKeyboardInput(KeyboardCode::KeyA, 1.0f);
    sideList.addKeyboardInput(KeyboardCode::KeyD, -1.0f);
    input->addInput(sideList, &inputControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((InputControl *)userData)->sideMove = -value; });

    InputDescriptorList rotateCameraXList;
    rotateCameraXList.addMouseInput(InputMouseType::MoveX, 1.0f);
    input->addInput(rotateCameraXList, &inputControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((InputControl *)userData)->rotateX = glm::clamp(-value, -64.0f, 64.0f); });

    InputDescriptorList rotateCameraYList;
    rotateCameraYList.addMouseInput(InputMouseType::MoveY, 1.0f);
    input->addInput(rotateCameraYList, &inputControl, [](InputType type, InputData *data, float value, void *userData)
                    { ((InputControl *)userData)->rotateY = glm::clamp(-value, -64.0f, 64.0f); });

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
    float time = 0;
    bool s = false;
    bool s1 = false;
    bool s2 = false;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();

        time += delta;
        if (time > 1.0f && !s)
        {
            s = true;
            boxComponent->destroy();
            boxComponent = nullptr;
        }

        if (time > 2.0f && !s1)
        {
            s1 = true;
            Red11::getResourceManager()->freeUnusedAll();
        }

        if (time > 3.0f && !s2)
        {
            s2 = true;
            Red11::getResourceManager()->freeUnusedAll();
            boxComponent = objectContainer->createComponentMesh(cubeMeshBig);
            boxComponent->setMaterial(crateMaterial);
            boxComponent->setPosition(randf(-3.0f, 3.0f), 0.8f, randf(-3.0f, 3.0f));
            Red11::getResourceManager()->freeUnusedAll();
        }

        if (boxComponent)
            boxComponent->setVisibility(!boxComponent->getVisibility());
        else
            objectContainer->setVisibility(!objectContainer->getVisibility());

        window->processWindow();

        window->setCursorVisibility(false);
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);

        renderer->prepareToRender();
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        cameraRX += inputControl.rotateY * 0.0015f;
        cameraRY += inputControl.rotateX * 0.0015f;
        cameraRX = glm::clamp(cameraRX, -1.2f, 1.0f);
        camera->setRotation(Vector3(cameraRX, cameraRY, 0));

        auto forward = camera->getRotation() * Vector3(0, 0, 0.8f);
        camera->translate(forward * delta * inputControl.move);
        auto side = camera->getRotation() * Vector3(0.8f, 0, 0);
        camera->translate(side * delta * inputControl.sideMove);

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        renderer->present();
    }

    return 0;
}