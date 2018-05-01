#pragma once

#include "UrhoIncludeAll.h"
#include "Ninja.h"

using namespace Urho3D;

class AutoAttackComponent : public LogicComponent {
URHO3D_OBJECT(AutoAttackComponent, LogicComponent);

    explicit AutoAttackComponent(Context *context) : LogicComponent(context) {}

    Entity *RaycastEntity() {
        float maxDistance = 100;
        UI *ui = GetSubsystem<UI>();
        IntVector2 pos = ui->GetCursorPosition();
        // Check the cursor is visible and there is no UI element in front of the cursor
        if (ui->GetElementAt(pos, true))
            return NULL;

        Entity *entity = NULL;

        auto *graphics = GetSubsystem<Graphics>();
        auto *camera = GetScene()->GetChild("Camera")->GetComponent<Camera>();
        Ray cameraRay = camera->GetScreenRay((float) pos.x_ / graphics->GetWidth(),
                                             (float) pos.y_ / graphics->GetHeight());

        PODVector<RayQueryResult> results;
        RayOctreeQuery query(results, cameraRay, RAY_OBB, maxDistance, DRAWABLE_ANY);
        GetScene()->GetComponent<Octree>()->Raycast(query);
        if (results.Size()) {
            for (const auto &result : results) {
                entity = result.node_->GetDerivedComponent<Entity>();
                if (entity != NULL) {
                    break;
                }
            }
        }
        return entity;
    }

    void Attack() {
        auto *entity = GetNode()->GetDerivedComponent<Entity>();

        // Get new target if none.
        if (currentTarget.Get() == NULL) {
            Entity *enemy = RaycastEntity();
            if (enemy != NULL && enemy->GetNode() != GetNode()) {
                currentTarget = enemy->GetNode();
            }
        }

        Node *targetNode = currentTarget.Get();

        if (targetNode != NULL) {
            auto *targetBody = targetNode->GetComponent<RigidBody>();

            float deltaX = 0.0f;
            float deltaY = 0.0f;

            Vector3 ownPos(GetNode()->GetPosition());
            const Vector3 &targetPos(targetNode->GetPosition());
            float distance = (targetPos - ownPos).Length();

            if (distance < 2.f) {
                entity->Attack(targetNode);
            } else {
                entity->Move(targetPos - ownPos, 1);
            }
        }
    }

    void StopAttack() {
        currentTarget = NULL;
    }

    WeakPtr<Node> currentTarget;
};

