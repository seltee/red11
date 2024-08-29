// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

struct UserRequest
{
    char userName[16];
};

struct UserResponse
{
    char responseToUser[32];
};

struct InputControl
{
    float move;
    float sideMove;
    float rotateX;
    float rotateY;
};

struct ApiFunctions
{
    NetworkApiCall userRequest;
    NetworkApiCall userResponse;
};

class MessageReceiverUser : public MessageReceiver
{
public:
    MessageReceiverUser()
    {
    }

    void receiveMessage(NetworkApiCall code, int size, const char *body) override final
    {
        if (code == apiFunctions.userResponse)
        {
            std::string answer = std::string(body);
            Red11::getLogger()->logConsole("Received: %s", answer.c_str());
        }
    }

    static ApiFunctions apiFunctions;
};
ApiFunctions MessageReceiverUser::apiFunctions;

MessageReceiver *funcCreateConnectionUser()
{
    return new MessageReceiverUser();
}

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Network", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);
    concreteMaterial->setRoughness(0.6f);

    auto hdr = new TextureFileHDR("HDR", "./data/meadow.hdr", 1.8f, 2.9f);
    auto radiance = hdr->getRadianceTexture();

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.1f, 0.12f, 0.14f));

    // Objects
    auto objectContainer = scene->createActor<Actor>();

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

    InputControl inputControl;
    memset(&inputControl, 0, sizeof(InputControl));

    // Network
    const int port = 8081;

    ApiFunctions apiFunctions;
    NetworkApi api = NetworkApi(0);
    apiFunctions.userRequest = api.addFixedSizeApiCall(sizeof(UserRequest));
    apiFunctions.userResponse = api.addFixedSizeApiCall(sizeof(UserResponse));
    MessageReceiverUser::apiFunctions = apiFunctions;

    MessageReceiverUser messageReceiver;
    Client *client = Red11::createClient(api, messageReceiver, "127.0.0.1", port);
    client->connectToServer();

    UserRequest userRequestData;
    memset(userRequestData.userName, 0, sizeof(UserRequest));
    memcpy(userRequestData.userName, "John", strlen("John"));
    client->request(apiFunctions.userRequest, &userRequestData, sizeof(UserRequest));

    // UI
    UI *ui = new UI(window, renderer, font);

    UINode *message = ui->createUINodeChild<UINode>();
    message->width.setAsPercentage(100.0f);
    message->height.setAsPercentage(100.0f);
    message->fontSize.set(48);
    message->colorText.set(Color(0.9f, 0.9f, 0.9f, 1.0f));
    message->letterSpacing.set(1.0f);
    message->setPaddingNumber(64.0f);
    message->text.set("Connecting to server ...");

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

    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();

        window->processWindow();

        window->setCursorVisibility(false);
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);

        renderer->prepareToRender(hdr, radiance);
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));
        renderer->renderCubeMap(cameraComponent->getCamera(), cameraComponent, hdr);

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

        if (client->hasErrors())
        {
            message->text.set("Error connecting to server");
        }
        else if (client->isConnected())
        {
            message->text.set("Connected to server");
        }

        ui->process(delta);
        ui->render();

        renderer->present();
    }

    return 0;
}