#pragma once

#include "UrhoIncludeAll.h"
#include "Entity.h"

using namespace Urho3D;

class FireComponent : public LogicComponent {
URHO3D_OBJECT(FireComponent, LogicComponent);
public:
    explicit FireComponent(Context *context) : LogicComponent(context) {
    }

protected:

public:
    void Start() override {
        LogicComponent::Start();
        auto particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/Fire.xml");
        particleEmitter->SetEffect(effect);
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        auto *entity = GetNode()->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            entity->Damage((int) timeStep * FIRE_DAMAGE_PER_MILLISECOND);
        }
    }

};

class GreenFireComponent : public LogicComponent {
URHO3D_OBJECT(GreenFireComponent, LogicComponent);
public:
    explicit GreenFireComponent(Context *context) : LogicComponent(context) {
    }

protected:

public:
    void Start() override {
        LogicComponent::Start();
        auto particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/GreenFire.xml");
        particleEmitter->SetEffect(effect);
    }

};

class BlueFireComponent : public LogicComponent {
URHO3D_OBJECT(BlueFireComponent, LogicComponent);
public:
    explicit BlueFireComponent(Context *context) : LogicComponent(context) {
    }

protected:

public:
    void Start() override {
        LogicComponent::Start();
        auto particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/BlueFire.xml");
        particleEmitter->SetEffect(effect);
    }

};

class PurpleFireComponent : public LogicComponent {
URHO3D_OBJECT(PurpleFireComponent, LogicComponent);
public:
    explicit PurpleFireComponent(Context *context) : LogicComponent(context) {
    }

protected:

public:
    void Start() override {
        LogicComponent::Start();
        auto particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/PurpleFire.xml");
        particleEmitter->SetEffect(effect);
    }

};
