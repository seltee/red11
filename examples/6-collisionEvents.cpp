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
};

enum class ObjectType
{
    Floor,
    ShootBall,
    BigBall
};

struct Context
{
    Audio *audioSystem;
    SoundFile *soundBong;
};

struct ObjectDescriptor
{
    ObjectType type;
    Context *context;
    Component *component;
    std::string name;
    bool bIsSharedDescriptor;
    float health;
};

class CollisionHandlerExample : public CollisionHandler
{
public:
    void collisionStarted(BodyCollisionData *data) override final
    {
        ObjectDescriptor *descA = (ObjectDescriptor *)data->bodyA->getUserData();
        ObjectDescriptor *descB = (ObjectDescriptor *)data->bodyB->getUserData();
        if (descA && descB)
        {
            if (descA->type == ObjectType::BigBall || descB->type == ObjectType::BigBall)
            {
                Audio *audioSystem = descA->context->audioSystem;
                audioSystem->playSound3d(descA->context->soundBong, data->pointA, 0.85f, 6.0f, 0.5f);
            }
        }

        process(data, " hit ");
    }
    void collisionPersisted(BodyCollisionData *data) override final
    {
        process(data, " touching ");
    }
    void collisionEnded(BodyCollisionData *data) override final
    {
        process(data, " stopped touching ");
    }

protected:
    inline void process(BodyCollisionData *data, std::string eventName)
    {
        ObjectDescriptor *descA = (ObjectDescriptor *)data->bodyA->getUserData();
        ObjectDescriptor *descB = (ObjectDescriptor *)data->bodyB->getUserData();
        if (descA && descB)
        {
            if (data->bodyA->getMotionType() == PhysicsMotionType::Dynamic && descA->component)
            {
                descA->health -= 0.01f;
                Red11::getLogger()->logConsole((descB->name + eventName + descA->name + "(Health: " + std::to_string(descA->health) + ")").c_str());
                if (descA->health < 0.0f)
                {
                    descA->component->destroy();
                    descA->component = nullptr;
                    if (!descA->bIsSharedDescriptor)
                    {
                        delete descA;
                        data->bodyA->setUserData(nullptr);
                    }
                }
            }
            if (data->bodyB->getMotionType() == PhysicsMotionType::Dynamic && descB->component)
            {
                descB->health -= 0.01f;
                Red11::getLogger()->logConsole((descA->name + eventName + descB->name + "(Health: " + std::to_string(descB->health) + ")").c_str());
                if (descB->health < 0.0f)
                {
                    descB->component->destroy();
                    descB->component = nullptr;
                    if (!descB->bIsSharedDescriptor)
                    {
                        delete descB;
                        data->bodyB->setUserData(nullptr);
                    }
                }
            }
        }
    }
};

APPMAIN
{
    Red11::openConsole();

    auto window = Red11::createWindow("Collisions Events Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    window->setCursorVisibility(false);
    auto renderer = Red11::createRenderer(window, RendererType::DirectX9);

    // Audio
    Audio *audioSystem = Red11::getAudio();
    auto list = audioSystem->getDevicesList();
    for (auto &item : list)
        Red11::getLogger()->logConsole("Audio device: %s, %s", item.name.c_str(), item.bIsDefault ? "default" : "selectable");
    audioSystem->setup();

    SoundFile *soundBong = new SoundFile("./data/metal.wav");
    SoundFile *soundShoot = new SoundFile("./data/shoot.wav");
    SoundFile *music = new SoundFile("./data/streamable.ogg");

    AudioSource *musicSource = audioSystem->createAudioSource();
    musicSource->loop(music);
    musicSource->setVolume(0.2f);

    Context context;
    context.audioSystem = audioSystem;
    context.soundBong = soundBong;

    // Textures & Materials
    auto concreteTexture = new TextureFile("Concrete", "./data/concrete_albedo.jpg");
    auto concreteMaterial = new MaterialSimple(concreteTexture);
    concreteMaterial->setRoughness(0.6f);

    auto redBallMaterial = new MaterialSimple(Color(0.8f, 0.1f, 0.1f), Color(0, 0, 0), 0.0f, 0.5f);
    auto yellowBallMaterial = new MaterialSimple(Color(0.91f, 0.604f, 0.067f), Color(0, 0, 0), 0.0f, 0.5f);
    auto greenBallMaterial = new MaterialSimple(Color(0.1f, 0.8f, 0.1f), Color(0, 0, 0), 0.0f, 0.5f);
    auto shootBallMaterial = new MaterialSimple(Color(0.8f, 0.8f, 0.8f), Color(0, 0, 0), 0.0f, 0.2f);

    // Meshes
    auto sphereMesh = Red11::getMeshBuilder()->createSphere(0.81f, 36, 28);
    auto ballSphereMesh = Red11::getMeshBuilder()->createSphere(0.041f);
    auto cubeMesh = Red11::getMeshBuilder()->createCube(0.1f);

    // Font
    Font *font = new Font("./data/Roboto-Medium.ttf");

    // Scene
    auto scene = Red11::createScene();
    scene->setAmbientLight(Color(0.4f, 0.4f, 0.6f));
    PhysicsWorld *world = scene->getPhysicsWorld();
    world->setGravity(Vector3(0, -1.0f, 0));

    // Base Descriptors
    ObjectDescriptor descriptorFloor = ObjectDescriptor({ObjectType::Floor, &context, nullptr, "Floor", true});
    ObjectDescriptor descriptorRedBall = ObjectDescriptor({ObjectType::BigBall, &context, nullptr, "Red Static Ball", true});
    ObjectDescriptor descriptorYellowBall = ObjectDescriptor({ObjectType::BigBall, &context, nullptr, "Yellow Static Ball", true});
    ObjectDescriptor descriptorGreenBall = ObjectDescriptor({ObjectType::BigBall, &context, nullptr, "Green Static Ball", true});

    CollisionHandlerExample *collisionHandler = scene->createCollisionHandler<CollisionHandlerExample>();

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

    // Objects
    auto objectContainer = scene->createActor<Actor>();

    auto sphereForm = world->createPhysicsForm(0.9f, 0.4f);
    sphereForm->createSphere(Vector3(0), 0.8f, 20.0f);

    auto ballSphereFrom = world->createPhysicsForm(0.4f, 0.65f);
    ballSphereFrom->createSphere(Vector3(0), 0.04f, 25.0f);

    auto generateSphere = [&](const Vector3 &center, MaterialSimple *material, ObjectDescriptor *descriptor)
    {
        auto sphere = objectContainer->createComponentMesh(sphereMesh);
        sphere->setMaterial(material);
        sphere->setPosition(center);
        sphere->enableCollisions(PhysicsMotionType::Static, sphereForm, descriptor);
        sphere->setCollisionHandler(collisionHandler);
    };

    generateSphere(Vector3(1.5f, 0.0f, -1.5f), redBallMaterial, &descriptorRedBall);
    generateSphere(Vector3(-1.5f, 0.0f, -1.5f), yellowBallMaterial, &descriptorYellowBall);
    generateSphere(Vector3(0.0f, 0.0f, 1.2f), greenBallMaterial, &descriptorGreenBall);

    // Floor
    auto floor = scene->createActor<Actor>();
    auto floorComponent = floor->createComponent<Component>();
    auto floorForm = world->createPhysicsForm(0.9f, 0.1f);
    floorForm->createPlain(Vector3(0, 1, 0), 0.0f);
    floorComponent->enableCollisions(PhysicsMotionType::Static, floorForm, &descriptorFloor);
    floorComponent->setRenderDebugPhysicsBody(true);
    floorComponent->setCollisionHandler(collisionHandler);

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
        fpsMeter->setText(std::string("FPS: ") + std::to_string(deltaCounter.getFPS()));

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
            sphereComponent->setMaterial(shootBallMaterial);
            ObjectDescriptor *descriptor = new ObjectDescriptor({ObjectType::ShootBall, &context, sphereComponent, "Shoot Ball", false, 4.0f});
            sphereComponent->enableCollisions(PhysicsMotionType::Dynamic, ballSphereFrom, descriptor);
            sphereComponent->getPhysicsBody()->addLinearVelocity(camera->getForwardVector() * 3.0f);
            audioSystem->playSound(soundShoot, 0.5f);
        }

        scene->process(delta);
        audioSystem->syncPosition(cameraComponent);
        scene->render(renderer, cameraComponent->getCamera());
        audioSystem->process(delta);

        renderer->present();
    }

    return 0;
}