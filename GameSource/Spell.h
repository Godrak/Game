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
        shield->ChangeShieldPower(SHIELD_POWER);
    }
};

class FireEffect : public Effect {
URHO3D_OBJECT(FireEffect, Effect);
public:
    FireEffect(Context *context) : Effect(context) {}

    void ApplyEffect(Node *node) override {
        node->CreateComponent<FireComponent>();
    }
};

class HealingEffect : public Effect {
URHO3D_OBJECT(HealingEffect, Effect);
public:
    HealingEffect(Context *context) : Effect(context) {}

    void ApplyEffect(Node *node) override {
        node->CreateComponent<HealingComponent>();
    }
};


class SpellBase : public LogicComponent {
URHO3D_OBJECT(SpellBase, LogicComponent);
public:
    explicit SpellBase(Context *context) : LogicComponent(context) {}

    virtual void ActivateNextSpell(Vector3 position) {
        if (nextSpell != NULL) {
            nextSpell->ActivateSpell(position, casterNode);
        }
    };

    void SetAdditionalEffect(Effect *effect) {
        additionalEffect = effect;
    }

    void SetNextSpell(SpellBase *spell) {
        nextSpell = spell;
    }

    virtual void ActivateSpell(Vector3 position, Node *caster) = 0;

protected:
    bool active = false;
    bool removed = false;
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    Node *spellNode{};
    StaticModel *staticModel{};
    RigidBody *rigidBody{};
    CollisionShape *collisionShape{};
    Node *casterNode = NULL;

    Effect *additionalEffect{};
    SpellBase *nextSpell{};
};

class EndOfSpell : public SpellBase {
URHO3D_OBJECT(EndOfSpell, SpellBase);
public:
    EndOfSpell(Context *context) : SpellBase(context) {}

    void SetSpellNodeToRemove(Node *node) {
        spellNode = node;
    }

    void ActivateSpell(Vector3 position, Node *caster) override {
        if (spellNode != NULL) {
            spellNode->Remove();
        }
    }

private:
    Node *spellNode;
};

class CasterTarget : public SpellBase {
URHO3D_OBJECT(CasterTarget, SpellBase);
public:
    CasterTarget(Context *context) : SpellBase(context) {}

    void ActivateSpell(Vector3 position, Node *caster) override {
        casterNode = caster;
        if (caster != NULL && additionalEffect != NULL) {
            additionalEffect->ApplyEffect(caster);
        }
        ActivateNextSpell(position);
    }
};

class Projectile : public SpellBase {
URHO3D_OBJECT(Projectile, SpellBase);
public:
    explicit Projectile(Context *context) : SpellBase(context) {}

    void ActivateSpell(Vector3 position, Node *caster) override {
        spellNode = GetScene()->CreateTemporaryChild();

        auto direction = Vector3::UP;
        auto casterPosition = Vector3::ZERO;
        if (caster != NULL) {
            casterPosition = caster->GetWorldPosition();
            direction = position - casterPosition;
            direction.y_ = 0;
            direction.Normalize();
        }

        spellNode->SetWorldPosition(casterPosition + direction + Vector3::UP);
        spellNode->SetScale(PROJECTILE_SCALE);

        rigidBody = spellNode->CreateComponent<RigidBody>();
        rigidBody->SetMass(1.f);
        rigidBody->SetCollisionLayer(1);
        rigidBody->ApplyImpulse(direction * PROJECTILE_STRENGTH);


        staticModel = spellNode->CreateComponent<StaticModel>();
        auto *sphereModel = cache->GetResource<Model>("Models/Sphere.mdl");
        auto *stoneMaterial = cache->GetResource<Material>("Materials/StoneBlack.xml");
        staticModel->SetModel(sphereModel);
        staticModel->SetMaterial(stoneMaterial);

        collisionShape = spellNode->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_SPHERE);
        if (additionalEffect != NULL) {
            additionalEffect->ApplyEffect(spellNode);
        }

        SubscribeToEvent(spellNode, E_NODECOLLISIONSTART, URHO3D_HANDLER(Projectile, HandleNodeCollision));
        active = true;
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        using namespace NodeCollision;
        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        auto *entity = otherNode->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            auto *otherNodeRigidBody = entity->GetComponent<RigidBody>();
            if (otherNodeRigidBody != NULL)
                entity->Damage((rigidBody->GetLinearVelocity() - otherNodeRigidBody->GetLinearVelocity()).Length());
            if (additionalEffect != NULL)
                additionalEffect->ApplyEffect(otherNode);
        }
    }

    void Update(float timeStep) override {
        if (!removed && active) {
            if (rigidBody->GetLinearVelocity().Length() < PROJECTILE_MIN_LINEAR_VELOCITY_LENGTH) {
                ActivateNextSpell(spellNode->GetPosition());
                removed = true;
                spellNode->Remove();
            }
        }
    }
};

class Explosion : public SpellBase {
URHO3D_OBJECT(Explosion, SpellBase);
public:
    Explosion(Context *context) : SpellBase(context) {}

    void ActivateSpell(Vector3 position, Node *caster) override {
        casterNode = caster;

        spellNode = GetScene()->CreateTemporaryChild();
        spellNode->SetWorldPosition(position);
        spellNode->SetScale(EXPLOSION_SCALE);

        auto *particleEmitter = spellNode->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/Explosion.xml");
        particleEmitter->SetEffect(effect);

        rigidBody = spellNode->CreateComponent<RigidBody>();
        rigidBody->SetCollisionLayer(1);
        rigidBody->SetTrigger(true);

        collisionShape = spellNode->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_SPHERE);

        SubscribeToEvent(spellNode, E_NODECOLLISIONSTART, URHO3D_HANDLER(Explosion, HandleNodeCollision));
        active = true;
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        using namespace NodeCollision;
        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        auto *entity = otherNode->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            auto *otherNodeRigidBody = entity->GetComponent<RigidBody>();
            Vector3 direction = otherNode->GetWorldPosition() - spellNode->GetWorldPosition();
            float strength = EXPLOSION_SCALE - direction.Length();
            entity->Damage(strength);
            otherNodeRigidBody->ApplyImpulse(direction.Normalized() * strength * EXPLOSION_STRENGTH_MULTIPLIER);
            if (additionalEffect != NULL)
                additionalEffect->ApplyEffect(otherNode);
        }
    }

    void Update(float timeStep) override {
        if (!active) return;
        time += timeStep;
        if (!removed && time > EXPLOSION_TIME_LIMIT) {
            ActivateNextSpell(spellNode->GetPosition());
            removed = true;
            spellNode->Remove();
        }
    }

private:
    float time = 0;

};