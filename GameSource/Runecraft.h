#pragma once

#include "UrhoIncludeAll.h"
#include "InputController.h"
#include "CameraController.h"
#include "TimeOutComponent.h"
#include "FireBurstComponent.h"
#include "DrawableTexture.h"

using namespace Urho3D;

class Runecraft : public Application
{
    URHO3D_OBJECT(Runecraft, Application);

public:
	explicit Runecraft(Context* context);

    void Setup() override;

    void Start() override;

private:
    void InitScene();
    void CreateCamera();

    Scene *scene_;
    Node *cameraNode_;

    void CreatePlayer(Vector3 position);

    void HandleKeyDown(StringHash eventType,VariantMap &eventData);

    void CreateMutant(Vector3 position);
};

URHO3D_DEFINE_APPLICATION_MAIN(Runecraft);