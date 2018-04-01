#pragma once

#include "UrhoIncludeAll.h"
#include "Entity.h"

using namespace Urho3D;

class Ninja : public Entity {
public:
URHO3D_OBJECT(Ninja, Entity);

    explicit Ninja(Context *context) : Entity(context) {}

    void Start() override {
        Entity::Start();

        auto *animatedModel = GetNode()->CreateComponent<AnimatedModel>();
        auto cache = GetSubsystem<ResourceCache>();
        auto *model = cache->GetResource<Model>("Models/Ninja/Ninja.mdl");
        auto *material = cache->GetResource<Material>("Models/Ninja/Materials/Ninja.xml");
        animatedModel->SetCastShadows(true);
        animatedModel->SetModel(model);
        animatedModel->SetMaterial(material);
        GetNode()->CreateComponent<AnimationController>();

        auto *rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetMass(1);
        rigidBody->SetCollisionLayer(1);
        rigidBody->SetAngularFactor(Vector3::ZERO);

        auto *collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(ShapeType::SHAPE_CAPSULE);
        collisionShape->SetCapsule(0.7, 1.8, Vector3(0, 0.9, 0));

        health->SetHealth(100);
        maxSpeed = 0.3f;
        walkAnim = "Models/Ninja/Ninja_Walk.ani";
        idleAnim = "Models/Ninja/Ninja_Idle3.ani";
    }
};

