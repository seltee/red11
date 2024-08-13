// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "red11.h"
#include <string>

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

struct CameraControl
{
    float move;
    float sideMove;
    float rotateX;
    float rotateY;
    bool shoot;
    bool remove;
};

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Physics Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    window->setCursorVisibility(false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Textures & Materials
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);
    concreteMaterial->setRoughness(0.6f);

    auto redBallMaterial = new MaterialSimple(Color(0.8f, 0.1f, 0.1f), Color(0, 0, 0), 0.0f, 0.4f);
    auto capsuleMaterial = new MaterialSimple(Color(0.91f, 0.604f, 0.067f), Color(0, 0, 0), 0.0f, 0.08f);
    auto buildingMaterial = new MaterialSimple(Color(0.75f, 0.75f, 0.75f), Color(0, 0, 0), 0.0f, 0.6f);

    auto crateTexture = new TextureFile("Crate", "./data/crate.jpg");
    auto crateMaterial = new MaterialSimple(crateTexture);
    crateMaterial->setRoughness(0.7f);
    crateMaterial->setMetallic(0.2f);

    auto hdr = new TextureFileHDR("HDR", "./data/meadow.hdr", 1.8f, 2.9f);
    auto radiance = hdr->getRadianceTexture();

    // Meshes
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);
    auto cubeMeshBig = Red11::getMeshBuilder()->createCube(0.25f);
    auto sphereMesh = Red11::getMeshBuilder()->createSphere(0.101f);
    auto ballSphereMesh = Red11::getMeshBuilder()->createSphere(0.041f);
    auto capsuleMesh = Red11::getMeshBuilder()->createCapsule(Vector3(0, -0.2f, 0), Vector3(0, 0.2f, 0), 0.09f, 16);
    auto polyMeshFileData = new Data3DFile("./data/poly_mesh.fbx");
    auto polyMesh = polyMeshFileData->getAsMesh();

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.1f, 0.12f, 0.14f));
    PhysicsWorld *world = scene->getPhysicsWorld();
    world->setGravity(Vector3(0, -1.0f, 0));

    // Floor
    for (int iy = 0; iy < 9; iy++)
    {
        for (int ix = 0; ix < 9; ix++)
        {
            auto floorCube = scene->createActor<Actor>();
            auto floorCubeComponent = floorCube->createComponentMesh(cubeMesh);
            floorCubeComponent->setMaterial(concreteMaterial);
            floorCubeComponent->setPosition(Vector3((float)(ix - 4) * 0.8f, 0.0f, (float)(iy - 4) * 0.8f));
            floorCubeComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
        }
    }

    // Sphere
    auto objectContainer = scene->createActor<Actor>();

    auto sphereComponent = objectContainer->createComponentMesh(sphereMesh);
    sphereComponent->setMaterial(concreteMaterial);
    sphereComponent->setPosition(0.0f, 0.3f, -0.025f);
    sphereComponent->setRenderDebugPhysicsBody(true);

    auto sphereComponent2 = objectContainer->createComponentMesh(sphereMesh);
    sphereComponent2->setMaterial(concreteMaterial);
    sphereComponent2->setPosition(0.0f, 0.15f, -0.02f);
    sphereComponent2->setRenderDebugPhysicsBody(true);

    auto sphereForm = world->createPhysicsForm(0.9f, 0.4f);
    sphereForm->createSphere(Vector3(0), 0.1f, 20.0f);

    auto ballSphereFrom = world->createPhysicsForm(0.4f, 0.65f);
    ballSphereFrom->createSphere(Vector3(0), 0.04f, 25.0f);

    // Boxes box & convex physics shapes
    auto boxForm = world->createPhysicsForm(0.9f, 0.15f);
    ShapeOBB *obb = boxForm->createOBB(Vector3(0), 0.25f, 16.0f);

    float halfWidth = obb->getHalfWidth();
    float halfHeight = obb->getHalfHeight();
    float halfDepth = obb->getHalfDepth();

    Vector3 points[8];
    points[0] = Vector3(halfWidth, halfHeight, halfDepth);
    points[1] = Vector3(halfWidth, halfHeight, -halfDepth);
    points[2] = Vector3(-halfWidth, halfHeight, halfDepth);
    points[3] = Vector3(-halfWidth, halfHeight, -halfDepth);
    points[4] = Vector3(halfWidth, -halfHeight, halfDepth);
    points[5] = Vector3(halfWidth, -halfHeight, -halfDepth);
    points[6] = Vector3(-halfWidth, -halfHeight, halfDepth);
    points[7] = Vector3(-halfWidth, -halfHeight, -halfDepth);

    auto boxFormConvex = world->createPhysicsForm(0.9f, 0.15f);
    boxFormConvex->createConvex(points, 8, obb->getPolygons(), 6);

    for (int i = 0; i < 3; i++)
    {
        auto boxComponent = objectContainer->createComponentMesh(cubeMeshBig);
        boxComponent->setMaterial(capsuleMaterial);
        boxComponent->setPosition(randf(-3.0f, 3.0f), 0.8f, randf(-3.0f, 3.0f));
        boxComponent->enableCollisions(PhysicsMotionType::Dynamic, boxFormConvex, boxComponent);
        boxComponent->setRenderDebugPhysicsBody(true);
    }

    for (int i = 0; i < 8; i++)
    {
        auto boxComponent = objectContainer->createComponentMesh(cubeMeshBig);
        boxComponent->setMaterial(crateMaterial);
        boxComponent->setPosition(randf(-3.0f, 3.0f), 0.8f, randf(-3.0f, 3.0f));
        boxComponent->enableCollisions(PhysicsMotionType::Dynamic, boxForm, boxComponent);
        boxComponent->setRenderDebugPhysicsBody(true);
    }

    // Capsules
    auto capsuleFrom = world->createPhysicsForm(0.9f, 0.15f);
    capsuleFrom->createCapsule(Vector3(0, -0.2f, 0), Vector3(0, 0.2f, 0), 0.09f, 20.0f);

    for (int i = 0; i < 1; i++)
    {
        auto capsuleComponent = objectContainer->createComponentMesh(capsuleMesh);
        capsuleComponent->setMaterial(capsuleMaterial);
        capsuleComponent->setPosition(randf(-3.0f, 3.0f), 1.0f, randf(-3.0f, 3.0f));
        capsuleComponent->enableCollisions(PhysicsMotionType::Dynamic, capsuleFrom);
        capsuleComponent->setRenderDebugPhysicsBody(true);
    }

    // Static Mesh
    auto meshFrom = world->createPhysicsForm(0.8f, 0.18f);
    meshFrom->createMesh(polyMesh);

    auto polyMeshComponent = objectContainer->createComponentMesh(polyMesh);
    polyMeshComponent->setMaterial(buildingMaterial);
    polyMeshComponent->enableCollisions(PhysicsMotionType::Static, meshFrom, nullptr, CHANNEL_SIMULATION);
    polyMeshComponent->setRenderDebugPhysicsBody(true);
    polyMeshComponent->setScale(0.0012f);
    polyMeshComponent->setPosition(Vector3(0, 0, -2));

    // Floor && walls
    auto floor = scene->createActor<Actor>();
    auto floorComponent = floor->createComponent<Component>();
    auto floorForm = world->createPhysicsForm(0.9f, 0.1f);
    floorForm->createPlain(Vector3(0, 1, 0), 0.0f);
    floorComponent->enableCollisions(PhysicsMotionType::Static, floorForm);
    floorComponent->setRenderDebugPhysicsBody(true);

    auto wallRightComponent = floor->createComponentMesh(cubeMesh);
    wallRightComponent->setMaterial(concreteMaterial);
    wallRightComponent->setRotation(Vector3(CONST_PI / 2.0f, 0.0f, 0.0f));
    wallRightComponent->setPosition(Vector3(0, 0.5, -3.5));
    wallRightComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
    wallRightComponent->enableCollisions(PhysicsMotionType::Static, floorForm);
    wallRightComponent->setRenderDebugPhysicsBody(true);

    auto wallLeftComponent = floor->createComponentMesh(cubeMesh);
    wallLeftComponent->setMaterial(concreteMaterial);
    wallLeftComponent->setRotation(Vector3(-CONST_PI / 2.0f, 0.0f, 0.0f));
    wallLeftComponent->setPosition(Vector3(0, 0.5, 3.5));
    wallLeftComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
    wallLeftComponent->enableCollisions(PhysicsMotionType::Static, floorForm);
    wallLeftComponent->setRenderDebugPhysicsBody(true);

    auto wallForwardComponent = floor->createComponentMesh(cubeMesh);
    wallForwardComponent->setMaterial(concreteMaterial);
    wallForwardComponent->setRotation(Vector3(CONST_PI / 2.0f, CONST_PI / 2.0f, 0.0f));
    wallForwardComponent->setPosition(Vector3(3.5, 0.5, 0));
    wallForwardComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
    wallForwardComponent->enableCollisions(PhysicsMotionType::Static, floorForm);
    wallForwardComponent->setRenderDebugPhysicsBody(true);

    auto wallBackwardComponent = floor->createComponentMesh(cubeMesh);
    wallBackwardComponent->setMaterial(concreteMaterial);
    wallBackwardComponent->setRotation(Vector3(CONST_PI / 2.0f, -CONST_PI / 2.0f, 0.0f));
    wallBackwardComponent->setPosition(Vector3(-3.5, 0.5, 0));
    wallBackwardComponent->setScale(Vector3(8.0f, 0.01f, 8.0f));
    wallBackwardComponent->enableCollisions(PhysicsMotionType::Static, floorForm);
    wallBackwardComponent->setRenderDebugPhysicsBody(true);

    // Light
    auto lightSun = scene->createActor<Actor>();
    auto lightSunComponent = lightSun->createComponent<ComponentLight>();
    lightSunComponent->setupDirectional(glm::normalize(Vector3(-1.0f, -1.0f, -1.0)), Color(3.8f, 3.4f, 3.2f), true, LightShadowQuality::Maximum);

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

    InputDescriptorList shootBallList;
    shootBallList.addMouseInput(InputMouseType::LeftButton, 1.0f);
    shootBallList.addKeyboardInput(KeyboardCode::KeyQ, 1.0f);
    input->addInput(shootBallList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    {
                        if (value > 0.5f){
                            ((CameraControl *)userData)->shoot = true;
                        } });

    InputDescriptorList removeEntityList;
    removeEntityList.addMouseInput(InputMouseType::RightButton, 1.0f);
    removeEntityList.addKeyboardInput(KeyboardCode::KeyE, 1.0f);
    input->addInput(removeEntityList, &cameraControl, [](InputType type, InputData *data, float value, void *userData)
                    {
                        if (value > 0.5f){
                            ((CameraControl *)userData)->remove = true;
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
    float timer = 0.5f;
    while (!window->isCloseRequested())
    {
        float delta = deltaCounter.getDeltaFrameCounter();
        fpsMeter->setText(std::string("FPS: ") + std::to_string(deltaCounter.getFPS()));

        window->processWindow();
        window->setMousePosition(renderer->getViewWidth() / 2, renderer->getViewHeight() / 2);
        renderer->prepareToRender(hdr, radiance);
        cameraComponent->setupAsPerspective(renderer->getViewWidth(), renderer->getViewHeight());
        renderer->clearBuffer(Color(0.4, 0.5, 0.8));
        renderer->renderCubeMap(cameraComponent->getCamera(), cameraComponent, hdr);

        timer -= delta;
        if (timer < 0.0f)
        {
            sphereComponent->enableCollisions(PhysicsMotionType::Dynamic, sphereForm, sphereComponent);
            sphereComponent2->enableCollisions(PhysicsMotionType::Dynamic, sphereForm, sphereComponent2);
            for (int i = 0; i < 16; i++)
            {
                auto sphereComponentI = objectContainer->createComponentMesh(sphereMesh);
                sphereComponentI->setMaterial(concreteMaterial);
                sphereComponentI->setPosition(randf(-3.0f, 3.0f), randf(1.5f, 3.0f), randf(-3.0f, 3.0f));
                sphereComponentI->enableCollisions(PhysicsMotionType::Dynamic, sphereForm, sphereComponentI);
            }
            timer = 100000.0f;
        }

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
            sphereComponent->enableCollisions(PhysicsMotionType::Dynamic, ballSphereFrom, sphereComponent);
            sphereComponent->getPhysicsBody()->addLinearVelocity(camera->getForwardVector() * 3.0f);
        }
        if (cameraControl.remove)
        {
            cameraControl.remove = false;
            std::vector<PhysicsBodyPoint> points = scene->castRayCollision(camera->getPosition(), camera->getPosition() + camera->getForwardVector() * 10.0f, CHANNEL_RAY_PICK, true);
            if (points.size() > 0)
            {
                for (auto &point : points)
                {
                    if (point.userData)
                    {
                        ((Component *)point.userData)->destroy();
                        break;
                    }
                }
            }
        }

        scene->process(delta);
        scene->render(renderer, cameraComponent->getCamera());

        renderer->present();
    }

    return 0;
}