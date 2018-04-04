#include "UrhoIncludeAll.h"
#include "FireComponent.h"
#include "TimeOutComponent.h"
#include "Entity.h"
#include "DrawableTexture.h"
#include "CameraController.h"
#include "SpellSystem.h"
#include "Caster.h"

#pragma once

using namespace Urho3D;

class InputController : public LogicComponent {
URHO3D_OBJECT(InputController, LogicComponent);
public:
    explicit InputController(Context *context) : LogicComponent(context) {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputController, HandleUpdate));
    }

    void HandleUpdate(StringHash eventType, VariantMap &eventData) {
        using namespace Update;
        auto *input = GetSubsystem<Input>();
        auto *entity = GetNode()->GetDerivedComponent<Entity>();
        auto *caster = GetNode()->GetComponent<Caster>();
        auto timeStep = eventData[P_TIMESTEP].GetFloat();

        if (entity != NULL) {
            Vector3 moveVector;
            if (input->GetKeyDown(KEY_W)) {
                moveVector += Vector3::FORWARD;
            }
            if (input->GetKeyDown(KEY_A)) {
                moveVector += Vector3::LEFT;
            }
            if (input->GetKeyDown(KEY_S)) {
                moveVector += Vector3::BACK;
            }
            if (input->GetKeyDown(KEY_D)) {
                moveVector += Vector3::RIGHT;
            }
            entity->Move(moveVector, 1);
        }

        if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
            rightMouseButtonDownFlag = true;
            caster->moveRectangle(GetNode()->GetPosition(), input->GetMouseMoveWheel());
        } else if (rightMouseButtonDownFlag) {
            rightMouseButtonDownFlag = false;
            auto *drawableTexture = GetScene()->GetChild("Plane")->GetComponent<DrawableTexture>();
            caster->cast(drawableTexture);
        }

        if (!input->GetMouseButtonDown(MOUSEB_RIGHT)) {
            GetNode()->GetComponent<CameraController>()->Zoom(input->GetMouseMoveWheel());
        }

        if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
            caster->draw(timeStep);
        } else {
            caster->endCurve();
        }
    }

private:
    bool rightMouseButtonDownFlag = false;

};
