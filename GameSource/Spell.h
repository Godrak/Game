#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include "Entity.h"
#include "States.h"

using namespace Urho3D;
using namespace ImageAnalyzer;

class Projectile : public LogicComponent {
URHO3D_OBJECT(Projectile, LogicComponent);
public:
    Projectile(Context *context) : LogicComponent(context) {}

    void Fire(Vector3 position,Vector3 direction) {
        GetNode()->SetWorldPosition(position+PROJECTILE_SCALE*direction+Vector3::UP);
        GetNode()->SetScale(PROJECTILE_SCALE);
        direction.Normalize();

        rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetMass(PROJECTILE_MASS);
        rigidBody->SetCollisionLayer(1);
        rigidBody->ApplyImpulse(direction * PROJECTILE_STRENGTH);


        auto* staticModel = GetNode()->CreateComponent<StaticModel>();
        auto *sphereModel = cache->GetResource<Model>("Models/Sphere.mdl");
        auto *stoneMaterial = cache->GetResource<Material>("Materials/StoneBlack.xml");
        staticModel->SetModel(sphereModel);
        staticModel->SetMaterial(stoneMaterial);

        auto* collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_SPHERE);

        SubscribeToEvent(GetNode(),E_NODECOLLISIONEND, URHO3D_HANDLER(Projectile, HandleNodeCollision));
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        using namespace NodeCollision;
        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        auto *entity = otherNode->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            auto *otherNodeRigidBody = entity->GetComponent<RigidBody>();
            if (otherNodeRigidBody != NULL)
                entity->Damage((rigidBody->GetLinearVelocity() - otherNodeRigidBody->GetLinearVelocity()).Length());
        }
    }

    void Update(float timeStep) override {
        if (!removed) {
            if (rigidBody->GetLinearVelocity().Length() < PROJECTILE_MIN_LINEAR_VELOCITY_LENGTH) {
                removed = true;
                GetNode()->Remove();
            }
        }
    }
private:
    RigidBody* rigidBody;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    bool removed = false;
};


class Explosion : public LogicComponent {
URHO3D_OBJECT(Explosion, LogicComponent);
public:
    Explosion(Context *context) : LogicComponent(context) {}

    void Explode(Vector3 position) {
        GetNode()->SetWorldPosition(position);
        GetNode()->SetScale(EXPLOSION_SCALE);

        auto *particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/Explosion.xml");
        particleEmitter->SetEffect(effect);

        rigidBody = GetNode()->CreateComponent<RigidBody>();
        rigidBody->SetCollisionLayer(1);
        rigidBody->SetTrigger(true);

        auto* collisionShape = GetNode()->CreateComponent<CollisionShape>();
        collisionShape->SetShapeType(SHAPE_SPHERE);

        SubscribeToEvent(GetNode(), E_NODECOLLISIONSTART, URHO3D_HANDLER(Explosion, HandleNodeCollision));
    }

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
        using namespace NodeCollision;
        auto *otherNode = (Node *) eventData[P_OTHERNODE].GetPtr();
        auto *entity = otherNode->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            auto *otherNodeRigidBody = entity->GetComponent<RigidBody>();
            Vector3 direction = otherNode->GetWorldPosition() - GetNode()->GetWorldPosition();
            float strength = EXPLOSION_SCALE - direction.Length();
            entity->Damage(strength*EXPLOSION_DAMAGE_MULTIPLIER);
            otherNodeRigidBody->ApplyImpulse(direction.Normalized() * strength * EXPLOSION_STRENGTH_MULTIPLIER);
        }
    }

    void Update(float timeStep) override {
        time += timeStep;
        if (!removed && time > EXPLOSION_TIME_LIMIT) {
            removed = true;
            GetNode()->Remove();
        }
    }

private:
    float time = 0;
    RigidBody* rigidBody;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    bool removed = false;

};
