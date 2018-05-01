#pragma once

#include "UrhoIncludeAll.h"
#include "Spell.h"
#include "TimeOutComponent.h"
#include "Entity.h"
#include "Mutant.h"
#include "SimpleAI.h"

using namespace Urho3D;

class TotemEffect : public LogicComponent {
URHO3D_OBJECT(TotemEffect, LogicComponent);
public:
    TotemEffect(Context *context) : LogicComponent(context) {}

    virtual void ApplyEffect(Node *node) {};

    virtual void Activate(Node *node) {};
};


class Totem : public LogicComponent {
URHO3D_OBJECT(Totem, LogicComponent);
public:
    explicit Totem(Context *context) : LogicComponent(context) {}

    virtual void Activate(Vector3 position) {
        rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetMass(0); // static object
        rigidBody->SetCollisionLayer(1);

        collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_CYLINDER);

        staticModel = GetNode()->CreateComponent<StaticModel>();
        auto *totemModel = cache->GetResource<Model>(modelPath);
        auto *material = cache->GetResource<Material>(materialPath);
        staticModel->SetModel(totemModel);
        staticModel->SetMaterial(material);

        areaOfEffectNode = GetNode()->CreateTemporaryChild("aoe");
        areaOfEffectNode->SetWorldPosition(GetNode()->GetWorldPosition());
        areaOfEffectNode->SetScale(areaOfEffectScale);

        aoeRigidBody = areaOfEffectNode->CreateComponent<RigidBody>();
        aoeRigidBody->SetCollisionLayer(1);
        aoeRigidBody->SetTrigger(true);

        aoeCollisionShape = areaOfEffectNode->CreateComponent<CollisionShape>();
        aoeCollisionShape->SetShapeType(SHAPE_SPHERE);

        aoeStaticModel = areaOfEffectNode->CreateComponent<StaticModel>();
        auto *sphereModel = cache->GetResource<Model>("Models/Sphere.mdl");
        aoeStaticModel->SetModel(sphereModel);
        material = cache->GetResource<Material>(aoeMaterialPath);
        aoeStaticModel->SetMaterial(material);

        GetNode()->SetWorldPosition(position);
        GetNode()->SetScale(TOTEM_SCALE);

        for (auto *effect: effects) {
            effect->Activate(GetNode());
        }

        SubscribeToEvent(areaOfEffectNode, E_NODECOLLISION, URHO3D_HANDLER(Totem, HandleNodeCollision));
    }

    void AddEffect(TotemEffect *effect) {
        effects.Push(effect);
    }

    void ExtendDuration(float value) {
        totemDuration += value;
    }

    void ExtendReach(float value) {
        areaOfEffectScale += value;
        areaOfEffectNode->SetScale(areaOfEffectScale);
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        using namespace NodeCollision;
        if (effectTriggerTime > 0) return;

        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        for (auto *effect: effects) {
            effect->ApplyEffect(otherNode);
        }
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        if (effectTriggerTime < -TOTEM_EFFECT_TRIGGER_WINDOW)
            effectTriggerTime = TOTEM_EFFECT_TRIGGER_TIME;
        effectTriggerTime = -timeStep;
        areaOfEffectNode->SetRotation((areaOfEffectNode->GetRotation() * Quaternion(0.2, Vector3::UP)).Normalized());
        totemDuration -= timeStep;
        if (!removed && totemDuration < 0) {
            removed = true;
            GetNode()->Remove();
        }
    }

protected:
    String modelPath = "Models/Totem/Cylinder.mdl";
    String materialPath = "Models/Totem/Material.xml";
    String aoeMaterialPath = "Materials/Elements.xml";
    Vector<TotemEffect *> effects{};

    ResourceCache *cache = GetSubsystem<ResourceCache>();
    StaticModel *staticModel{};
    RigidBody *rigidBody{};
    CollisionShape *collisionShape{};

    float effectTriggerTime = TOTEM_EFFECT_TRIGGER_TIME;
    float totemDuration = TOTEM_DURATION;
    bool removed = false;
    float areaOfEffectScale = TOTEM_EFFECT_SIZE;
    Node *areaOfEffectNode{};
    StaticModel *aoeStaticModel{};
    CollisionShape *aoeCollisionShape{};
    RigidBody *aoeRigidBody{};
};

//class ShieldTotem : public Totem {
//URHO3D_OBJECT(ShieldTotem, Totem);
//public:
//    ShieldTotem(Context *context) : Totem(context) {}
//
//    void Activate(Vector3 position) override {
//        Totem::Activate(position);
//        effect = areaOfEffectNode->CreateComponent<ShieldEffect>();
//    }
//
//    void ApplyEffects(Node *node) override {
//        if (node->GetName() == "Player") {
//            auto *entity = node->GetDerivedComponent<Entity>();
//            if (entity != NULL) {
//                effect->ApplyEffect(node);
//            }
//        }
//    }
//};

//class WindTotem : public Totem {
//URHO3D_OBJECT(WindTotem, Totem);
//public:
//    WindTotem(Context *context) : Totem(context) {}
//
//    void Activate(Vector3 position) override {
//        Totem::Activate(position);
//        effect = areaOfEffectNode->CreateComponent<FrozenEffect>();
//    }
//
//    void ApplyEffects(Node *node) override {
//        if (node->GetName() != "Player") {
//            auto *entity = node->GetDerivedComponent<Entity>();
//            if (entity != NULL) {
//                effect->ApplyEffect(node);
//            }
//        }
//    }
//};
//
//class StrengthTotem : public Totem {
//URHO3D_OBJECT(StrengthTotem, Totem);
//public:
//    StrengthTotem(Context *context) : Totem(context) {}
//
//    void Activate(Vector3 position) override {
//        Totem::Activate(position);
//        effect = areaOfEffectNode->CreateComponent<FireEffect>();
//    }
//
//    void ApplyEffects(Node *node) override {
//        if (node->GetName() == "Player") {
//            auto *entity = node->GetDerivedComponent<Entity>();
//            if (entity != NULL) {
//                effect->ApplyEffect(node);
//            }
//        }
//    }
//};
//
//class NatureTotem : public Totem {
//URHO3D_OBJECT(NatureTotem, Totem);
//public:
//    NatureTotem(Context *context) : Totem(context) {}
//
//    void Activate(Vector3 position) override {
//        Totem::Activate(position);
//        effect = areaOfEffectNode->CreateComponent<HealingEffect>();
//    }
//
//    void ApplyEffects(Node *node) override {
//        if (node->GetName() == "Player") {
//            auto *entity = node->GetDerivedComponent<Entity>();
//            if (entity != NULL) {
//                effect->ApplyEffect(node);
//            }
//        }
//    }
//};

class ShieldEffect : public TotemEffect {
URHO3D_OBJECT(ShieldEffect, TotemEffect);
public:
    ShieldEffect(Context *context) : TotemEffect(context) {}

    void ApplyEffect(Node *node) override {
        if (node->GetName() == PLAYER_NODE_NAME) {
            auto *shield = node->GetOrCreateComponent<ShieldComponent>();
            shield->ChangeShieldPower(SHIELD_POWER_CHARGE);
        }
    }
};

class StrengthEffect : public TotemEffect {
URHO3D_OBJECT(StrengthEffect, TotemEffect);
public:
    StrengthEffect(Context *context) : TotemEffect(context) {}

    void ApplyEffect(Node *node) override {
        if (node->GetName() == PLAYER_NODE_NAME) {
            node->GetOrCreateComponent<StrengthState>();
        }
    }
};


class FireEffect : public TotemEffect {
URHO3D_OBJECT(FireEffect, TotemEffect);
public:
    FireEffect(Context *context) : TotemEffect(context) {}

    void ApplyEffect(Node *node) override {
        if (node->GetName() == MUTANT_NODE_NAME)
            node->GetOrCreateComponent<FireState>();
    }
};

class HealingEffect : public TotemEffect {
URHO3D_OBJECT(HealingEffect, TotemEffect);
public:
    HealingEffect(Context *context) : TotemEffect(context) {}

    void ApplyEffect(Node *node) override {
        if (node->GetName() == PLAYER_NODE_NAME)
            node->GetOrCreateComponent<HealingState>();
    }
};

class FrozenEffect : public TotemEffect {
URHO3D_OBJECT(FrozenEffect, TotemEffect);
public:
    FrozenEffect(Context *context) : TotemEffect(context) {}

    void ApplyEffect(Node *node) override {
        if (node->GetName() == MUTANT_NODE_NAME)
            node->GetOrCreateComponent<FrozenState>();
    }
};

class DurabilityEffect : public TotemEffect {
URHO3D_OBJECT(DurabilityEffect, TotemEffect);
public:
    DurabilityEffect(Context *context) : TotemEffect(context) {}

    void Activate(Node *node) override {
        TotemEffect::Activate(node);
        Totem *totem = node->GetDerivedComponent<Totem>();
        if (totem != NULL) {
            totem->ExtendDuration(DURABILITY_EXTENSION);
        }
    }
};

class PowerEffect : public TotemEffect {
URHO3D_OBJECT(PowerEffect, TotemEffect);
public:
    PowerEffect(Context *context) : TotemEffect(context) {}

    void Activate(Node *node) override {
        TotemEffect::Activate(node);
        auto *totem = node->GetDerivedComponent<Totem>();
        if (totem != NULL) {
            totem->ExtendReach(REACH_EXTENSION);
        }
    }
};

class DistractionEffect : public TotemEffect {
URHO3D_OBJECT(DistractionEffect, TotemEffect);
public:
    DistractionEffect(Context *context) : TotemEffect(context) {}

    void ApplyEffect(Node *node) override {
        TotemEffect::ApplyEffect(node);
        auto *ai = node->GetComponent<SimpleAI>();
        if (ai != NULL) {
            ai->SetTarget(GetNode());
        }
    }
};



