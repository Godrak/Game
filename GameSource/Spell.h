#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/Params.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "FireComponent.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include "Entity.h"
#include <iostream>

using namespace Urho3D;
using namespace ImageAnalyzer;

class Effect : public LogicComponent {
URHO3D_OBJECT(Effect, LogicComponent);
public:
    Effect(Context *context) : LogicComponent(context) {}

    virtual void ApplyEffect(Node *node)= 0;

};

class ShieldEffect : public Effect {
URHO3D_OBJECT(ShieldEffect, Effect);
public:
    ShieldEffect(Context *context) : Effect(context) {}

    void ApplyEffect(Node *node) override {
        auto *shield = node->CreateComponent<ShieldComponent>();
        shield->ChangeShieldPower(shieldPower);
    }

private:
    int shieldPower = 100;
};

class FireEffect : public Effect {
    URHO3D_OBJECT(FireEffect, Effect);
public:
    FireEffect(Context *context) : Effect(context) {}

    void ApplyEffect(Node *node) override {

    }
};


class SpellBase : public LogicComponent {
URHO3D_OBJECT(SpellBase, LogicComponent);
public:
    SpellBase(Context *context) : LogicComponent(context) {
        SubscribeToEvent(E_NODECOLLISIONSTART, URHO3D_HANDLER(SpellBase, HandleNodeCollision));

    }

    virtual void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {}

protected:
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    StaticModel *staticModel;
    RigidBody *rigidBody;
    CollisionShape *collisionShape;
};


class Projectile : public SpellBase {
URHO3D_OBJECT(Projectile, SpellBase);
public:
    explicit Projectile(Context *context) : SpellBase(context) {}

    void SetUp(Vector3 force, Vector3 position) {
        GetNode()->SetPosition(position);
        GetNode()->SetScale(0.7f);

        staticModel = GetNode()->CreateComponent<StaticModel>();
        auto *sphereModel = cache->GetResource<Model>("Models/Sphere.mdl");
        auto *stoneMaterial = cache->GetResource<Material>("Materials/StoneBlack.xml");
        staticModel->SetModel(sphereModel);
        staticModel->SetMaterial(stoneMaterial);

        rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetMass(0.5f);
        rigidBody->SetCollisionLayer(1);
        rigidBody->ApplyImpulse(force);

        collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_SPHERE);
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) override {
        using namespace NodeCollision;
        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        auto *entity = otherNode->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            entity->Damage((int) rigidBody->GetAngularVelocity().Length());
        }
    }
};

class Spell : public LogicComponent {
URHO3D_OBJECT(Spell, LogicComponent);
public:
    explicit Spell(Context *context) : LogicComponent(context) {}
};