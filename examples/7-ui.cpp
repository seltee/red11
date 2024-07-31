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
    bool shoot;
    bool controlCamera;
    bool helpShown;
};

const unsigned char F = 255;
unsigned char arrowMaskRight[] = {
    0, 0, 0, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, F, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, F, F, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, F, F, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, F, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, F, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned char arrowMaskDown[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F,
    0, F, F, F, F, F, F, F, F, F, F, F, F, F, F, 0,
    0, 0, F, F, F, F, F, F, F, F, F, F, F, F, 0, 0,
    0, 0, 0, F, F, F, F, F, F, F, F, F, F, 0, 0, 0,
    0, 0, 0, 0, F, F, F, F, F, F, F, F, 0, 0, 0, 0,
    0, 0, 0, 0, 0, F, F, F, F, F, F, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, F, F, F, F, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, F, F, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

class UIUserEventController : public UIEventController
{
public:
    void setup(InputControl *inputControl)
    {
        this->inputControl = inputControl;
    }

    void processEvent(UIEvent ev, UINode *node) override final
    {
        UIUserData *data = (UIUserData *)node->userData;
        if (!data)
            return;

        if (ev == UIEvent::Click)
        {
            if (data->button == UIButtonID::AddBall)
                inputControl->shoot = true;

            if (data->button == UIButtonID::Help || data->button == UIButtonID::Modal)
                inputControl->helpShown = !inputControl->helpShown;
        }
    }

    InputControl *inputControl = nullptr;
};

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("UI Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);
    concreteMaterial->setRoughness(0.6f);

    auto redBallMaterial = new MaterialSimple(Color(0.8f, 0.1f, 0.1f), Color(0, 0, 0), 0.0f, 0.4f);

    auto crateTexture = new TextureFile("Crate", "./data/crate.jpg");
    auto crateMaterial = new MaterialSimple(crateTexture);
    crateMaterial->setRoughness(0.7f);
    crateMaterial->setMetallic(0.2f);

    auto arrowRightTexture = new Texture("ArrowRight", TextureType::ByteMap, 16, 16, arrowMaskRight);
    auto arrowDownTexture = new Texture("ArrowDown", TextureType::ByteMap, 16, 16, arrowMaskDown);

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);
    auto cubeMeshBig = Red11::getMeshBuilder()->createCube(0.25f);
    auto ballSphereMesh = Red11::getMeshBuilder()->createSphere(0.041f);

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.4f, 0.4f, 0.6f));
    PhysicsWorld *world = scene->getPhysicsWorld();
    world->setGravity(Vector3(0, -1.0f, 0));

    // Floor
    for (int iy = 0; iy < 9; iy++)
    {
        for (int ix = 0; ix < 9; ix++)
        {
            auto floorCube = scene->createActor<Actor>("FloorCell");
            auto floorCubeComponent = floorCube->createComponentMesh(cubeMesh);
            floorCubeComponent->setMaterial(concreteMaterial);
            floorCubeComponent->setPosition(Vector3((float)(ix - 4) * 0.8f, 0.0f, (float)(iy - 4) * 0.8f));
            floorCubeComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
        }
    }

    // Sphere
    auto objectContainer = scene->createActor<Actor>("ObjectContainer");

    auto sphereForm = world->createPhysicsForm(0.9f, 0.4f);
    sphereForm->createSphere(Vector3(0), 0.1f, 20.0f);

    auto ballSphereFrom = world->createPhysicsForm(0.4f, 0.65f);
    ballSphereFrom->createSphere(Vector3(0), 0.04f, 25.0f);

    // Boxes box & convex physics shapes
    auto boxForm = world->createPhysicsForm(0.9f, 0.15f);
    boxForm->createOBB(Vector3(0), 0.25f, 16.0f);

    for (int i = 0; i < 8; i++)
    {
        auto boxComponent = objectContainer->createComponentMesh(cubeMeshBig);
        boxComponent->setMaterial(crateMaterial);
        boxComponent->setPosition(randf(-3.0f, 3.0f), 0.8f, randf(-3.0f, 3.0f));
        boxComponent->enablePhysics(PhysicsMotionType::Dynamic, boxForm, boxComponent);
        boxComponent->setRenderDebugPhysicsBody(true);
    }

    // Floor
    auto floor = scene->createActor<Actor>("Floor");
    auto floorComponent = floor->createComponent<Component>();
    auto floorForm = world->createPhysicsForm(0.9f, 0.1f);
    floorForm->createPlain(Vector3(0, 1, 0), 0.0f);
    floorComponent->enablePhysics(PhysicsMotionType::Static, floorForm);
    floorComponent->setRenderDebugPhysicsBody(true);

    // Light
    auto lightSun = scene->createActor<Actor>("Light");
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(3.8f, 3.4f, 3.2f), true, LightShadowQuality::Maximum);

    // Camera
    Actor *camera = scene->createActor<Actor>("Camera");
    ComponentCamera *cameraComponent = camera->createComponent<ComponentCamera>();
    camera->setPosition(0, 0.2, 0);

    InputControl inputControl;
    memset(&inputControl, 0, sizeof(InputControl));

    // UI
    UI *ui = new UI(window, renderer, font);
    UIUserEventController *uiController = ui->createEventController<UIUserEventController>();
    uiController->setup(&inputControl);

    UINode *canvas = ui->createUINodeChild<UINode>();
    canvas->width.setAsPercentage(100.0f);
    canvas->height.setAsPercentage(100.0f);
    canvas->contentDirection.set(UIContentDirection::Vertical);
    canvas->fontSize.set(48);
    canvas->colorText.set(Color(0.9f, 0.9f, 0.9f, 1.0f));
    canvas->letterSpacing.set(1.0f);

    UINode *topBar = canvas->createUINodeChild<UINode>();
    topBar->width.setAsPercentage(100.0f);
    topBar->height.setAsNumber(60.0f);
    topBar->colorBackground.set(Color(0.2f, 0.2f, 0.2f, 1.0f));

    UINode *screen = canvas->createUINodeChild<UINode>();
    screen->width.setAsPercentage(100.0f);
    screen->height.setAsPercentage(100.0f);
    screen->contentDirection.set(UIContentDirection::Horizontal);
    screen->horizontalAlign.set(UIContentAlign::SpaceAround);
    screen->verticalAlign.set(UIContentAlign::SpaceAround);
    screen->positioning.set(UIBlockPositioning::Relative);

    UINode *helpContainer = screen->createUINodeChild<UINode>();
    helpContainer->colorBackground.set(Color(0, 0, 0, 0.6));
    helpContainer->positioning.set(UIBlockPositioning::Absolute);
    helpContainer->width.setAsPercentage(100);
    helpContainer->height.setAsPercentage(100);
    helpContainer->horizontalAlign.set(UIContentAlign::Middle);
    helpContainer->verticalAlign.set(UIContentAlign::Middle);
    helpContainer->triggersEventClick = true;
    helpContainer->visibility.set(false);
    helpContainer->userData = new UIUserData(UIButtonID::Modal);

    UINode *helpModal = helpContainer->createUINodeChild<UINode>();
    helpModal->setPaddingNumber(48.0f, 72.0f);
    helpModal->colorBackground.set(Color(0.25, 0.25, 0.25, 1));
    helpModal->horizontalAlign.set(UIContentAlign::Middle);
    helpModal->verticalAlign.set(UIContentAlign::Middle);
    helpModal->contentDirection.set(UIContentDirection::Vertical);

    UINode *title = helpModal->createUINodeChild<UINode>();
    title->text.set("Title");
    title->getMarginBottom().setAsNumber(8);
    title->fontSize.set(72);

    UINode *line1 = helpModal->createUINodeChild<UINode>();
    line1->text.set("Help information about this example");

    UINode *line2 = helpModal->createUINodeChild<UINode>();
    line2->text.set("has no information");

    auto createMenuButton = [&](std::string text, UIButtonID button, std::vector<std::string> *menu = nullptr)
    {
        bool hasArrow = menu && menu->size() > 0;

        UIUserData *data = new UIUserData(button);

        UINode *barButton = topBar->createUINodeChild<UINode>(data);
        barButton->height.setAsPercentage(100);
        barButton->setPaddingNumber(16.0f, 0.0f);
        barButton->getStyleHover()->colorBackground.set(Color(0.4, 0.4, 0.4));
        barButton->verticalAlign.set(UIContentAlign::Middle);
        barButton->propagateHover.set(true);
        barButton->cursorIcon.set(MouseCursorIcon::Hand);
        barButton->triggersEventClick = true;
        barButton->positioning.set(UIBlockPositioning::Relative);

        UINode *barButtonText = barButton->createUINodeChild<UINode>();
        barButtonText->text.set(text);
        if (hasArrow)
        {
            UINode *barButtonImage = barButton->createUINodeChild<UINode>();
            barButtonImage->image.set(arrowRightTexture);
            barButtonImage->setMarginNumber(8.0f, 0, 0, 0);
            barButtonImage->colorImageMask.set(Color(0.9f, 0.9f, 0.9f, 1));
            barButtonImage->getStyleHover()->image.set(arrowDownTexture);
        }

        if (menu && menu->size() > 0)
        {
            UINode *dropDownMenu = barButton->createUINodeChild<UINode>(data);
            dropDownMenu->colorBackground.set(Color(0.2f, 0.2f, 0.2f, 1.0f));
            dropDownMenu->positioning.set(UIBlockPositioning::Absolute);
            dropDownMenu->getMarginTop().setAsNumber(60.0f);
            dropDownMenu->contentDirection.set(UIContentDirection::Vertical);
            dropDownMenu->fontSize.set(36);

            for (auto &item : *menu)
            {
                UINode *dropDownMenuItem = dropDownMenu->createUINodeChild<UINode>(data);
                dropDownMenuItem->width.setAsNumber(120);
                dropDownMenuItem->height.setAsNumber(32.0f);
                dropDownMenuItem->setPaddingNumber(12.0f, 16.0f);
                dropDownMenuItem->text.set(item);
                dropDownMenuItem->getStyleHover()->colorBackground.set(Color(0.4, 0.4, 0.4));
            }
        }
    };
    createMenuButton("File", UIButtonID::File);
    createMenuButton("Add ball", UIButtonID::AddBall);
    createMenuButton("Help", UIButtonID::Help);

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

    InputDescriptorList zoomInterfaceIn;
    zoomInterfaceIn.addMouseInput(InputMouseType::Wheel, 1.0f);
    input->addInput(zoomInterfaceIn, ui, [](InputType type, InputData *data, float value, void *userData)
                    { 
                        ((UI *)userData)->interfaceZoom += value * 0.25f;
                        ((UI *)userData)->interfaceZoom = fminf(fmaxf(((UI *)userData)->interfaceZoom, 0.5f), 3.0f); });

    InputDescriptorList controlCameraList;
    controlCameraList.addMouseInput(InputMouseType::RightButton, 1.0f);
    input->addInput(controlCameraList, &inputControl, [](InputType type, InputData *data, float value, void *userData)
                    { 
                        ((InputControl *)userData)->controlCamera = value > 0.5f; 
                        ((InputControl *)userData)->rotateX = 0.0f;
                        ((InputControl *)userData)->rotateY = 0.0f; });

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

        if (inputControl.controlCamera)
        {
            window->setCursorVisibility(false);
            window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);
        }
        else
            window->setCursorVisibility(true);

        renderer->prepareToRender();
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        if (inputControl.controlCamera)
        {
            cameraRX += inputControl.rotateY * 0.0015f;
            cameraRY += inputControl.rotateX * 0.0015f;
            cameraRX = glm::clamp(cameraRX, -1.2f, 1.0f);
            camera->setRotation(Vector3(cameraRX, cameraRY, 0));
        }

        auto forward = camera->getRotation() * Vector3(0, 0, 0.8f);
        camera->translate(forward * delta * inputControl.move);
        auto side = camera->getRotation() * Vector3(0.8f, 0, 0);
        camera->translate(side * delta * inputControl.sideMove);

        if (inputControl.shoot)
        {
            inputControl.shoot = false;
            auto sphereComponent = objectContainer->createComponentMesh(ballSphereMesh);
            sphereComponent->setPosition(camera->getPosition() - Vector3(0, 0.05f, 0) + camera->getForwardVector() * 0.2f);
            sphereComponent->setMaterial(redBallMaterial);
            sphereComponent->enablePhysics(PhysicsMotionType::Dynamic, ballSphereFrom, sphereComponent);
            sphereComponent->getPhysicsBody()->addLinearVelocity(camera->getForwardVector() * 3.0f);
        }

        helpContainer->visibility.set(inputControl.helpShown);

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        ui->process(delta);
        ui->render();

        renderer->present();
    }

    return 0;
}