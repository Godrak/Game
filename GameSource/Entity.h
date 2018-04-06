#pragma once

#include "UrhoIncludeAll.h"
#include "HitPointsComponent.h"
#include "ShieldComponent.h"

using namespace Urho3D;

class Entity : public LogicComponent {
URHO3D_OBJECT(Entity, LogicComponent);

    explicit Entity(Context *context) : LogicComponent(context) {}

    void Start() override {
        LogicComponent::Start();
        health = GetNode()->CreateComponent<HitPointsComponent>();
    }

    virtual void Heal(float value) {
        health->ChangeHealth(value);
    }

    virtual void Damage(float value) {
        if (shield != NULL) {
            shield->ChangeShieldPower(-value);
        } else if (!health->ChangeHealth(-value)) {
            Die();
        }
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        auto *rigidBody = GetNode()->GetComponent<RigidBody>();
        if (rigidBody != NULL && animationController != NULL /*&& idleAnim != NULL && walkAnim != NULL*/) {
            if (rigidBody->GetLinearVelocity().Length() > 0.5 && state == WALKING) {
                animationController->SetSpeed(walkAnim, rigidBody->GetLinearVelocity().Length() / 2.5f);
                animationController->PlayExclusive(walkAnim, 0, true, 0.5f);
            } else {
                state = IDLE;
                animationController->PlayExclusive(idleAnim, 0, true, 0.5f);
            }
        }
    }


    virtual void Attack() {};

    virtual void Move(Vector3 direction, float speed) {
        direction.y_ = 0;
        auto *nodeRigidBody = GetNode()->GetComponent<RigidBody>();
        if (nodeRigidBody != NULL) {
            if (direction.Length() != 0) {
                state = WALKING;
                nodeRigidBody->ApplyImpulse(direction.Normalized() * maxSpeed * speed);
                Quaternion newDirection;
                newDirection.FromLookRotation(direction, Vector3::UP);
                GetNode()->SetRotation(GetNode()->GetRotation().Slerp(newDirection, 0.15F));
            }
            auto velocity = nodeRigidBody->GetLinearVelocity();
            velocity.y_ = 0;
            Vector3 brakeForce = -velocity * 0.05f;
            nodeRigidBody->ApplyImpulse(brakeForce);
        }
    };

    virtual void Die() {
        GetNode()->Remove();
    }

    void SetSpeed(float speed) {
        maxSpeed = speed;
    }

    float GetSpeed() {
        return maxSpeed;
    }

protected:
    enum EntityState {
        IDLE, WALKING,
    };

    String walkAnim{};
    String idleAnim{};
    HitPointsComponent *health{};
    ShieldComponent *shield{};
    float maxSpeed{};
    EntityState state;
    AnimationController *animationController{};

};
