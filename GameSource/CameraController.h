#include "UrhoIncludeAll.h"

#pragma once

using namespace Urho3D;

class CameraController : public LogicComponent {
URHO3D_OBJECT(CameraController, LogicComponent);
public:
    explicit CameraController(Context *context) : LogicComponent(context) {}

    void Start() override {
        LogicComponent::Start();
        cameraNode = GetScene()->GetChild("Camera");
    }


    void Update(float timeStep) override {
        Vector3 oldPosition = cameraNode->GetPosition();
        auto newPosition = oldPosition;
        newPosition.y_ = bound(oldPosition.y_);
        if (cameraNode != NULL) {
            newPosition.x_ = GetNode()->GetPosition().x_;
            newPosition.z_ = GetNode()->GetPosition().z_;
            newPosition.z_ -= CAMERA_Y_AXIS_OFFSET * newPosition.y_;

            Quaternion newDirection;
            newDirection.FromLookRotation(GetNode()->GetPosition());
            cameraNode->SetRotation(cameraNode->GetRotation().Slerp(newDirection, 0.15F));
            cameraNode->SetPosition(newPosition);
            cameraNode->LookAt(GetNode()->GetPosition());
        }
    }

    void Zoom(float value) {
        Vector3 oldPosition = cameraNode->GetPosition();
        auto newPosition = oldPosition;
        if (value == 0)
            return;

        newPosition.y_ = bound(oldPosition.y_ + value);
        if (cameraNode != NULL) {
            newPosition.x_ = GetNode()->GetPosition().x_;
            newPosition.z_ = GetNode()->GetPosition().z_;
            newPosition.z_ -= CAMERA_Y_AXIS_OFFSET * newPosition.y_;

            Quaternion newDirection;
            newDirection.FromLookRotation(GetNode()->GetPosition());
            cameraNode->SetRotation(cameraNode->GetRotation().Slerp(newDirection, 0.15F));
            cameraNode->SetPosition(newPosition);
            cameraNode->LookAt(GetNode()->GetPosition());
        }
    }


private:
    Node *cameraNode{};


    float bound(float verticalPos) {
        if (verticalPos > MAX_VERTICAL_DISTANCE) return MAX_VERTICAL_DISTANCE;
        if (verticalPos < MIN_VERTICAL_DISTANCE) return MIN_VERTICAL_DISTANCE;
        return verticalPos;
    }
};
