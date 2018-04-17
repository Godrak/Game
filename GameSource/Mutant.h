#pragma once

#include "UrhoIncludeAll.h"
#include "Entity.h"

using namespace Urho3D;

class Mutant : public Entity {
URHO3D_OBJECT(Mutant, Entity);

    Mutant(Context *context) : Entity(context) {}

    void Start() override {
        Entity::Start();

        auto *cache = GetSubsystem<ResourceCache>();

        Node *adjustNode = GetNode()->CreateChild();
        Quaternion qAdjRot(180, Vector3(0, 1, 0)); // rotate it by 180
        adjustNode->SetRotation(qAdjRot);
        auto *animatedModel = adjustNode->CreateComponent<AnimatedModel>();
        animatedModel->SetModel(cache->GetResource<Model>("Models/Mutant/Mutant.mdl"));
        animatedModel->SetMaterial(cache->GetResource<Material>("Models/Mutant/Materials/mutant_M.xml"));
        animatedModel->SetCastShadows(true);
        animationController = adjustNode->CreateComponent<AnimationController>();

        auto *rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetMass(1);
        rigidBody->SetCollisionLayer(1);
        rigidBody->SetAngularFactor(Vector3::ZERO);

        auto *collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetCapsule(0.7, 1.8, Vector3(0, 0.9, 0));

        health->SetHealth(MUTANT_HEALTH);
        maxSpeed = MUTANT_SPEED;
        attackDamage = MUTANT_ATTACK_DAMAGE;
        idleAnim = "Models/Mutant/Mutant_Idle0.ani";
        walkAnim = "Models/Mutant/Mutant_Walk.ani";
        attackAnim = "Models/Mutant/Mutant_Swipe.ani";
    }

};
