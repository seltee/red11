// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 2560
#define WINDOW_HEIGHT 1440

struct CameraControl
{
    float move;
    float sideMove;
    float rotateX;
    float rotateY;
    bool shoot;
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

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("physics Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    window->setCursorVisibility(false);
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

    CameraControl cameraControl;
    memset(&cameraControl, 0, sizeof(CameraControl));

    // UI
    UI *ui = new UI(window, renderer, font);
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
    screen->setMarginNumber(10.0f, 10.0f, 10.0f, 10.0f);
    screen->contentDirection.set(UIContentDirection::Horizontal);
    screen->horizontalAlign.set(UIContentAlign::SpaceAround);
    screen->verticalAlign.set(UIContentAlign::SpaceAround);

    UINode *F = screen->createUINodeChild<UINode>();
    F->width.setAsNumber(120.0f);
    F->height.setAsNumber(120.0f);
    F->colorBackground.set(Color(0.8f, 0.2f, 0.2f, 0.8f));

    F = screen->createUINodeChild<UINode>();
    F->width.setAsNumber(100.0f);
    F->height.setAsNumber(100.0f);
    F->colorBackground.set(Color(0.2f, 0.8f, 0.2f, 0.8f));

    F = screen->createUINodeChild<UINode>();
    F->width.setAsNumber(140.0f);
    F->height.setAsNumber(140.0f);
    F->colorBackground.set(Color(0.2f, 0.2f, 0.8f, 0.8f));

    UINode *barMenu1 = topBar->createUINodeChild<UINode>();
    barMenu1->width.setAsNumber(160.0f);
    barMenu1->height.setAsNumber(300.0f);
    barMenu1->getMarginTop().setAsNumber(60);
    barMenu1->positioning.set(UIBlockPositioning::Absolute);
    barMenu1->colorBackground.set(Color(0.5f, 0.5f, 0.5f, 1.0f));

    UINode *barMenu2 = topBar->createUINodeChild<UINode>();
    barMenu2->width.setAsNumber(140.0f);
    barMenu2->height.setAsNumber(280.0f);
    barMenu2->getMarginTop().setAsNumber(60);
    barMenu2->positioning.set(UIBlockPositioning::Absolute);
    barMenu2->colorBackground.set(Color(0.8f, 0.8f, 0.8f, 1.0f));

    UINode *barButton1 = topBar->createUINodeChild<UINode>();
    barButton1->height.setAsPercentage(100);
    barButton1->setPaddingNumber(12.0f, 0.0f);
    barButton1->minWidth.setAsNumber(120.0f);
    barButton1->colorBackground.set(Color(0.2f, 0.8f, 0.8f, 1.0f));
    barButton1->verticalAlign.set(UIContentAlign::Middle);
    UINode *barButton1Text = barButton1->createUINodeChild<UINode>();
    barButton1Text->text.set("File");
    UINode *barButton1Image = barButton1->createUINodeChild<UINode>();
    barButton1Image->image.set(arrowRightTexture);
    barButton1Image->setMarginNumber(8.0f, 0, 0, 0);
    barButton1Image->colorImageMask.set(Color(0.9f, 0.9f, 0.9f, 1));

    UINode *barButton2 = topBar->createUINodeChild<UINode>();
    barButton2->height.setAsPercentage(100);
    barButton2->setPaddingNumber(12.0f, 0.0f);
    barButton2->minWidth.setAsNumber(120.0f);
    barButton2->colorBackground.set(Color(0.8f, 0.2f, 0.8f, 1.0f));
    barButton2->text.set("Add ball");
    barButton2->textVerticalAlign.set(UIContentAlign::Middle);

    UINode *barButton3 = topBar->createUINodeChild<UINode>();
    barButton3->height.setAsPercentage(100);
    barButton3->setPaddingNumber(12.0f, 0.0f);
    barButton3->minWidth.setAsNumber(120.0f);
    barButton3->colorBackground.set(Color(0.8f, 0.8f, 0.2f, 1.0f));
    barButton3->text.set("Help");
    barButton3->textVerticalAlign.set(UIContentAlign::Middle);

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

    InputDescriptorList shootBallList;
    shootBallList.addMouseInput(InputMouseType::LeftButton, 1.0f);
    shootBallList.addKeyboardInput(KeyboardCode::KeyQ, 1.0f);
    input->addInput(shootBallList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    {
                        if (value > 0.5f){
                            ((CameraControl *)userData)->shoot = true;
                        } });

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
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);
        renderer->prepareToRender();
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));

        cameraRX += cameraControl.rotateY * 0.0015f;
        cameraRY += cameraControl.rotateX * 0.0015f;
        cameraRX = glm::clamp(cameraRX, -1.2f, 1.0f);
        camera->setRotation(Vector3(cameraRX, cameraRY, 0));

        auto forward = camera->getRotation() * Vector3(0, 0, 0.8f);
        camera->translate(forward * delta * cameraControl.move);
        auto side = camera->getRotation() * Vector3(0.8f, 0, 0);
        camera->translate(side * delta * cameraControl.sideMove);

        if (cameraControl.shoot)
        {
            cameraControl.shoot = false;
            auto sphereComponent = objectContainer->createComponentMesh(ballSphereMesh);
            sphereComponent->setPosition(camera->getPosition() - Vector3(0, 0.05f, 0) + camera->getForwardVector() * 0.2f);
            sphereComponent->setMaterial(redBallMaterial);
            sphereComponent->enablePhysics(PhysicsMotionType::Dynamic, ballSphereFrom, sphereComponent);
            sphereComponent->getPhysicsBody()->addLinearVelocity(camera->getForwardVector() * 3.0f);
        }

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        ui->process(delta);
        ui->render();

        renderer->present();
    }

    return 0;
}