// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>
#include "renderer/directx9/directx9renderer.h"

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

struct InputControl
{
};

class Player : public Actor
{
public:
    Player() : Actor(std::string("Player"))
    {
        auto playerComponent = createComponentMesh(cubeMesh);
        playerComponent->setMaterial(new MaterialSimple(Color(0.9f, 0.2f, 0.2f)));
        playerComponent->setScale(0.8f, 0.8f, 1.2f);
        playerComponent->setPosition(0, 0.04f, 0);

        input = new InputProvider();

        // To use a gamepad with certain input you need to add receiveing input from specific gamepad to the input
        // Otherwise input events won't be triggered at all
        // You can bind to all gamepads if you want to
        auto gamePadList = window->getGamepadList();
        if (gamePadList.size() > 0)
            gamepad = gamePadList.at(0);
        if (gamepad)
            input->addGamepad(gamepad);

        // Any input is used to wait for any keypress
        // Use it to detect preferable input method
        input->addAnyKeyboardInput(this, [](void *userData)
                                   { reinterpret_cast<Player *>(userData)->setInputMethod(false); });
        input->addAnyMouseInput(this, [](void *userData)
                                { reinterpret_cast<Player *>(userData)->setInputMethod(false); });
        input->addAnyGamepadInput(this, [](void *userData)
                                  { reinterpret_cast<Player *>(userData)->setInputMethod(true); });

        InputDescriptorList forwardList;
        forwardList.addKeyboardInput(KeyboardCode::Up, 1.0f);
        forwardList.addKeyboardInput(KeyboardCode::KeyW, 1.0f);
        if (gamepad)
            forwardList.addGamepadButtonInput(gamepad->getDefaultCodeDPadUp(), 1.0f);
        input->addInput(forwardList, this, [](InputType type, InputData *data, float value, void *userData)
                        { reinterpret_cast<Player *>(userData)->setMoveForward(value); });

        InputDescriptorList backwardsList;
        backwardsList.addKeyboardInput(KeyboardCode::Down, 1.0f);
        backwardsList.addKeyboardInput(KeyboardCode::KeyS, 1.0f);
        if (gamepad)
            backwardsList.addGamepadButtonInput(gamepad->getDefaultCodeDPadDown(), 1.0f);
        input->addInput(backwardsList, this, [](InputType type, InputData *data, float value, void *userData)
                        { reinterpret_cast<Player *>(userData)->setMoveBackwards(value); });

        InputDescriptorList leftList;
        leftList.addKeyboardInput(KeyboardCode::Left, 1.0f);
        leftList.addKeyboardInput(KeyboardCode::KeyA, 1.0f);
        if (gamepad)
            leftList.addGamepadButtonInput(gamepad->getDefaultCodeDPadLeft(), 1.0f);
        input->addInput(leftList, this, [](InputType type, InputData *data, float value, void *userData)
                        { reinterpret_cast<Player *>(userData)->setMoveLeft(value); });

        InputDescriptorList rightList;
        rightList.addKeyboardInput(KeyboardCode::Right, 1.0f);
        rightList.addKeyboardInput(KeyboardCode::KeyD, 1.0f);
        if (gamepad)
            rightList.addGamepadButtonInput(gamepad->getDefaultCodeDPadRight(), 1.0f);
        input->addInput(rightList, this, [](InputType type, InputData *data, float value, void *userData)
                        { reinterpret_cast<Player *>(userData)->setMoveRight(value); });
    }

    void onProcess(float fDelta) override final
    {
        translate(Vector3(-(fMoveLeftValue - fMoveRightValue), 0, -(fMoveForwardValue - fMoveBackwardValue)) * fDelta * 0.4f);
    }

    void setInputMethod(bool bIsUsingController)
    {
        this->bIsUsingController = bIsUsingController;
    }

    void setMoveForward(float value)
    {
        fMoveForwardValue = value;
    }

    void setMoveBackwards(float value)
    {
        fMoveBackwardValue = value;
    }

    void setMoveLeft(float value)
    {
        fMoveLeftValue = value;
    }

    void setMoveRight(float value)
    {
        fMoveRightValue = value;
    }

    InputProvider *input = nullptr;
    Gamepad *gamepad = nullptr;
    bool bIsUsingController;

    float fMoveForwardValue = 0.0f;
    float fMoveBackwardValue = 0.0f;
    float fMoveLeftValue = 0.0f;
    float fMoveRightValue = 0.0f;

    static Mesh *cubeMesh;
    static Window *window;
};
Mesh *Player::cubeMesh = nullptr;
Window *Player::window = nullptr;

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Gamepad", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = (DirectX9Renderer *)Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);
    concreteMaterial->setRoughness(0.6f);

    auto hdr = new TextureFileHDR("HDR", "./data/meadow.hdr", 1.8f, 2.9f);
    auto radiance = hdr->getRadianceTexture();

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.1f, 0.12f, 0.14f));

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Objects
    auto objectContainer = scene->createActor<Actor>();

    // Player
    Player::cubeMesh = cubeMesh;
    Player::window = window;
    auto player = scene->createActor<Player>();

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
    Actor *camera = scene->createActor<Actor>();
    ComponentCamera *cameraComponent = camera->createComponent<ComponentCamera>();
    camera->setPosition(0, 0.2, 0);

    // Gamepads
    auto list = window->getGamepadList();

    InputControl inputControl;
    memset(&inputControl, 0, sizeof(InputControl));

    // UI
    UI *ui = new UI(window, renderer, font);

    UINode *canvas = ui->createUINodeChild<UINode>();
    canvas->width.setAsPercentage(100.0f);
    canvas->height.setAsPercentage(100.0f);
    canvas->contentDirection.set(UIContentDirection::Vertical);
    canvas->fontSize.set(48);
    canvas->colorText.set(Color(0.9f, 0.9f, 0.9f, 1.0f));
    canvas->verticalAlign.set(UIContentAlign::SpaceBetween);
    canvas->setPaddingNumber(40.0f);

    UINode *topBar = canvas->createUINodeChild<UINode>();
    topBar->width.setAsPercentage(100.0f);
    topBar->contentDirection.set(UIContentDirection::Vertical);

    UINode *title = topBar->createUINodeChild<UINode>();
    title->text.set("Gamepads list:");

    auto gamePadList = window->getGamepadList();
    if (gamePadList.size() > 0)
    {
        for (auto &item : gamePadList)
        {
            UINode *subtitle = topBar->createUINodeChild<UINode>();
            subtitle->text.set(item->getDisplayName());
        }
    }
    else
    {
        UINode *subtitle = topBar->createUINodeChild<UINode>();
        subtitle->text.set("None");
    }

    UINode *bottomBar = canvas->createUINodeChild<UINode>();
    bottomBar->width.setAsPercentage(100.0f);
    bottomBar->horizontalAlign.set(UIContentAlign::End);

    // Input
    auto input = Red11::getGlobalInputProvider();
    InputDescriptorList toggleFullscreen;
    toggleFullscreen.addKeyboardInput(KeyboardCode::KeyF, 1.0f);
    input->addInput(toggleFullscreen, window, [](InputType type, InputData *data, float value, void *userData)
                    { if (value > 0.5f) ((Window *)userData)->setFullscreen(!((Window *)userData)->isFullscreen()); });

    InputDescriptorList quitButtonList;
    quitButtonList.addKeyboardInput(KeyboardCode::Escape, 1.0f);
    input->addInput(quitButtonList, window, [](InputType type, InputData *data, float value, void *userData)
                    { ((Window *)userData)->close(); });

    DeltaCounter deltaCounter;
    float time = 0.0f;

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();
        time += delta * 0.001f;

        window->processWindow();

        window->setCursorVisibility(false);
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);

        auto engineData = renderer->getShaderEngineDataPtr();
        engineData[0] = time;
        renderer->prepareToRender(hdr, radiance);
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));
        renderer->renderCubeMap(cameraComponent->getCamera(), cameraComponent, hdr);

        Vector3 targetCameraPosition = player->getPosition() + Vector3(0, 0.5, 0.6f);
        Vector3 cameraDifference = targetCameraPosition - camera->getPosition();
        camera->setPosition(camera->getPosition() + cameraDifference * fminf(delta * 4.0f, 1.0f));
        camera->lookAt(player->getPosition());

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        bottomBar->text.set(player->bIsUsingController ? std::string("Using controller") : std::string("Using keyboard and mouse"));

        ui->process(delta);
        ui->render();

        renderer->present();
    }

    return 0;
}