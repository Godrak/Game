#pragma once

#include "UrhoIncludeAll.h"

using namespace Urho3D;

class FireBurstComponent : public LogicComponent {
URHO3D_OBJECT(FireBurstComponent, LogicComponent);
public:
    explicit FireBurstComponent(Context *context) : LogicComponent(context) {
    }

protected:

public:
    void Start() override {
        LogicComponent::Start();
        auto particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/Fire.xml");
        particleEmitter->SetEffect(effect);
    }

};

class GreenFireBurstComponent : public LogicComponent {
URHO3D_OBJECT(GreenFireBurstComponent, LogicComponent);
public:
    explicit GreenFireBurstComponent(Context *context) : LogicComponent(context) {
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

class BlueFireBurstComponent : public LogicComponent {
URHO3D_OBJECT(BlueFireBurstComponent, LogicComponent);
public:
    explicit BlueFireBurstComponent(Context *context) : LogicComponent(context) {
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

class PurpleFireBurstComponent : public LogicComponent {
URHO3D_OBJECT(PurpleFireBurstComponent, LogicComponent);
public:
    explicit PurpleFireBurstComponent(Context *context) : LogicComponent(context) {
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
