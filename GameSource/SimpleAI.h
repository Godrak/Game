#pragma once

#include "UrhoIncludeAll.h"
#include "Ninja.h"

using namespace Urho3D;

class SimpleAI : public LogicComponent {
URHO3D_OBJECT(SimpleAI, LogicComponent);

    explicit SimpleAI(Context *context) : LogicComponent(context) {}

    void GetNewTarget() {}

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        auto *entity = GetNode()->GetDerivedComponent<Entity>();

        // Get new target if none. Do not constantly scan for new targets to conserve CPU time
        if (currentTarget.Get() == NULL) {
            newTargetTimer += timeStep;
            if (newTargetTimer > 0.5)
                GetNewTarget();
        }

        Node *targetNode = currentTarget.Get();

        if (targetNode != NULL) {
            auto *targetBody = targetNode->GetComponent<RigidBody>();

            float deltaX = 0.0f;
            float deltaY = 0.0f;

            Vector3 ownPos(GetNode()->GetPosition());
            const Vector3 &targetPos(targetNode->GetPosition());
            float distance = (targetPos - ownPos).Length();

            if (distance < 0.2) {
                entity->Attack();
            } else {
                entity->Move(targetPos - ownPos,1);
            }
        } else {
            // If no target, walk idly
            dirChangeTime -= timeStep;
            entity->Move(rndDirection, 0.3f);
            if (dirChangeTime <= 0.0f) {
                dirChangeTime = 1.0f + Random(2.0f);
                if (Random(0.f, 1.f) > 0.7f)
                    rndDirection = Vector3::ZERO;
                else
                    rndDirection = Vector3(Random(-1.f, 1.f), 0, Random(-1.f, 1.f));
            }
        }
    }

    WeakPtr<Node> currentTarget;
    float newTargetTimer{};
    float dirChangeTime{};
    Vector3 rndDirection;

};

