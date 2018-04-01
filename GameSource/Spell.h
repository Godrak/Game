#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/Params.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "FireBurstComponent.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include <iostream>

using namespace Urho3D;
using namespace ImageAnalyzer;

class Effect : public LogicComponent {
URHO3D_OBJECT(Effect, LogicComponent);
public:
    Effect(Context *context) : LogicComponent(context) {}

    virtual void Activate(Vector3 position)= 0;

    void HandleNodeCollision(StringHash eventType, VariantMap& eventData)

protected:
    CollisionShape *collisionShape = NULL;
    RigidBody *rigidBody = NULL;
    StaticModel *staticModel = NULL;
};

class ProjectileEffect : public Effect {
URHO3D_OBJECT(ProjectileEffect, Effect);
public:
    ProjectileEffect(Context *context) : Effect(context) {}

    void SetUp(Vector3 force) {
        this->force = force;
    }

    void Activate(Vector3 position) override {
        GetNode()->SetPosition(position);
        GetNode()->SetScale(0.3f);

        auto *cache = GetSubsystem<ResourceCache>();
        staticModel = GetNode()->CreateComponent<StaticModel>();
        auto *sphereModel = cache->GetResource<Model>("models/Sphere.mdl");
        auto *stoneMaterial = cache->GetResource<Material>("materials/BlackStone.xml");
        staticModel->SetModel(sphereModel);
        staticModel->SetMaterial(stoneMaterial);

        auto *rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetMass(1);
        rigidBody->SetCollisionLayer(1);
        rigidBody->ApplyForce(force);

        //todo move collision shape to Spell
        auto *collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_SPHERE);

        //todo Subscribe to event

        //todo clear up
    }

private:
    Vector3 force;
};

class Spell : public LogicComponent {
URHO3D_OBJECT(Spell, LogicComponent);
public:
    explicit Spell(Context *context) : LogicComponent(context) {}
};