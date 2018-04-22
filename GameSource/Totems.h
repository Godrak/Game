#pragma once

#include "UrhoIncludeAll.h"
#include "TimeOutComponent.h"
#include "Entity.h"
#include "Spell.h"

using namespace Urho3D;

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
        areaOfEffectNode->SetScale(TOTEM_EFFECT_SIZE);

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

        SubscribeToEvent(areaOfEffectNode, E_NODECOLLISION, URHO3D_HANDLER(Totem, HandleNodeCollision));
    }

    virtual void ApplyEffect(Node *node)= 0;

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        using namespace NodeCollision;
        if (effectTriggerTime > 0) return;

        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        ApplyEffect(otherNode);
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        if (effectTriggerTime < -TOTEM_EFFECT_TRIGGER_WINDOW)
            effectTriggerTime = TOTEM_EFFECT_TRIGGER_TIME;
        effectTriggerTime = -timeStep;
        areaOfEffectNode->SetRotation((areaOfEffectNode->GetRotation() * Quaternion(0.2, Vector3::UP)).Normalized());
    }

protected:
    String modelPath = "Models/Totem/Cylinder.mdl";
    String materialPath = "Models/Totem/Material.xml";
    String aoeMaterialPath = "Materials/Elements.xml";
    Effect *effect{};

    ResourceCache *cache = GetSubsystem<ResourceCache>();
    StaticModel *staticModel{};
    RigidBody *rigidBody{};
    CollisionShape *collisionShape{};

    float effectTriggerTime = TOTEM_EFFECT_TRIGGER_TIME;
    Node *areaOfEffectNode{};
    StaticModel *aoeStaticModel{};
    CollisionShape *aoeCollisionShape{};
    RigidBody *aoeRigidBody{};
};

class ShieldTotem : public Totem {
URHO3D_OBJECT(ShieldTotem, Totem);
public:
    ShieldTotem(Context *context) : Totem(context) {}

    void Activate(Vector3 position) override {
        Totem::Activate(position);
        effect = areaOfEffectNode->CreateComponent<ShieldEffect>();
    }

    void ApplyEffect(Node *node) override {
        if (node->GetName() == "Player") {
            auto *entity = node->GetDerivedComponent<Entity>();
            if (entity != NULL) {
                effect->ApplyEffect(node);
            }
        }
    }
};

class WindTotem : public Totem {
URHO3D_OBJECT(WindTotem, Totem);
public:
    WindTotem(Context *context) : Totem(context) {}

    void Activate(Vector3 position) override {
        Totem::Activate(position);
        effect = areaOfEffectNode->CreateComponent<FrozenEffect>();
    }

    void ApplyEffect(Node *node) override {
        if (node->GetName() != "Player") {
            auto *entity = node->GetDerivedComponent<Entity>();
            if (entity != NULL) {
                effect->ApplyEffect(node);
            }
        }
    }
};

class StrengthTotem : public Totem {
URHO3D_OBJECT(StrengthTotem, Totem);
public:
    StrengthTotem(Context *context) : Totem(context) {}

    void Activate(Vector3 position) override {
        Totem::Activate(position);
        effect = areaOfEffectNode->CreateComponent<FireEffect>();
    }

    void ApplyEffect(Node *node) override {
        if (node->GetName() != "Player") {
            auto *entity = node->GetDerivedComponent<Entity>();
            if (entity != NULL) {
                effect->ApplyEffect(node);
            }
        }
    }
};

class NatureTotem : public Totem {
URHO3D_OBJECT(NatureTotem, Totem);
public:
    NatureTotem(Context *context) : Totem(context) {}

    void Activate(Vector3 position) override {
        Totem::Activate(position);
        effect = areaOfEffectNode->CreateComponent<HealingEffect>();
    }

    void ApplyEffect(Node *node) override {
        if (node->GetName() == "Player") {
            auto *entity = node->GetDerivedComponent<Entity>();
            if (entity != NULL) {
                effect->ApplyEffect(node);
            }
        }
    }
};


