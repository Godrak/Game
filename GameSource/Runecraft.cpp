#include "Runecraft.h"
#include "Ninja.h"
#include "Mutant.h"
#include "SimpleAI.h"

Runecraft::Runecraft(Context *context) : Application(context) {
    context->RegisterFactory<InputController>();
    context->RegisterFactory<CameraController>();
    context->RegisterFactory<DrawableTexture>();
    context->RegisterFactory<Caster>();
    context->RegisterFactory<SpellSystem>();
    context->RegisterFactory<Curve>();

    context->RegisterFactory<Ninja>();
    context->RegisterFactory<Mutant>();
    context->RegisterFactory<HitPointsComponent>();
    context->RegisterFactory<ShieldComponent>();
    context->RegisterFactory<SimpleAI>();
    context->RegisterFactory<TimeOutComponent>();

    context->RegisterFactory<ShieldTotem>();
    context->RegisterFactory<WindTotem>();
    context->RegisterFactory<StrengthTotem>();
    context->RegisterFactory<NatureTotem>();

    context->RegisterFactory<FireEffect>();
    context->RegisterFactory<HealingEffect>();
    context->RegisterFactory<ShieldEffect>();
    context->RegisterFactory<FrozenEffect>();
    context->RegisterFactory<StrengthEffect>();

    context->RegisterFactory<FireState>();
    context->RegisterFactory<FrozenState>();
    context->RegisterFactory<StrengthState>();
    context->RegisterFactory<HealingState>();

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Runecraft, HandleKeyDown));
}

void Runecraft::Setup() {
    // Modify engine startup parameters
    engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
    engineParameters_[EP_LOG_NAME] =
            GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_HEADLESS] = false;

    // Construct a search path to find the resource prefix with two entries:
    // The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
    // The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
    if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS))
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";../share/Resources;../share/Urho3D/Resources";
}

void Runecraft::Start() {
    GetSubsystem<Input>()->SetMouseMode(MM_ABSOLUTE);
    GetSubsystem<Input>()->SetMouseVisible(true);

    InitScene();
    CreateCamera();
    CreatePlayer(Vector3(0.0f, 3.0f, 0.0f));
    CreateMutant(Vector3(10, 3, 0));

}

void Runecraft::InitScene() {
    ResourceCache *cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);

    // Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
    // show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
    // is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
    // optimizing manner
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<PhysicsWorld>();
    scene_->CreateComponent<SpellSystem>();

    // Create a child scene node (at world origin) and a StaticModel component into it. Set the StaticModel to show a simple
    // plane mesh with a "stone" material. Note that naming the scene nodes is optional. Scale the scene node larger
    // (100 x 100 world units)
    Node *planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    auto *planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/Terrain.xml"));

    planeNode->CreateComponent<RigidBody>();

    planeNode->CreateComponent<DrawableTexture>();

    auto *shape = planeNode->CreateComponent<CollisionShape>();
    shape->SetBox(Vector3(1, 0, 1));

    // Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    // light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    // The light will use default settings (white light, no shadows)
    Node *lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    auto *light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
}

void Runecraft::CreateCamera() {
    cameraNode_ = scene_->CreateChild("Camera", CreateMode::LOCAL);
    cameraNode_->CreateComponent<Camera>();
    // Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f, 30.0f, -10.0f));
    cameraNode_->SetRotation(Quaternion(70.0f, 0.0f, 0.0f));

    auto *renderer = GetSubsystem<Renderer>();
    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    // at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    // use, but now we just use full screen and default render path configured in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

}

void Runecraft::CreatePlayer(Vector3 position) {
    Node *playerNode = scene_->CreateChild("Player");
    playerNode->SetPosition(position);

    playerNode->CreateComponent<InputController>();
    playerNode->CreateComponent<CameraController>();
    playerNode->CreateComponent<Mutant>();
    playerNode->CreateComponent<Caster>();
}


void Runecraft::HandleKeyDown(StringHash eventType, VariantMap &eventData) {
    using namespace KeyDown;

    auto *input = GetSubsystem<Input>();
    if (input->GetKeyDown(KEY_ESCAPE)) {
        engine_->Exit();
    }
}

void Runecraft::CreateMutant(Vector3 position) {
    auto *cache = GetSubsystem<ResourceCache>();

    auto *mutantNode = scene_->CreateChild("Mutant");
    mutantNode->SetPosition(position);

    auto *mutant = mutantNode->CreateComponent<Mutant>();
    mutant->SetSpeed(MUTANT_SPEED * 0.7f);

//    mutantNode->CreateComponent<SimpleAI>();
}
