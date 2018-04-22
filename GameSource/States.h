#pragma once

#include "UrhoIncludeAll.h"
#include "Entity.h"

using namespace Urho3D;

class FireState : public LogicComponent {
URHO3D_OBJECT(FireState, LogicComponent);
public:
    explicit FireState(Context *context) : LogicComponent(context) {
    }

    void Start() override {
        LogicComponent::Start();
        particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/Fire.xml");
        particleEmitter->SetEffect(effect);
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        auto *entity = GetNode()->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            entity->Damage(timeStep * FIRE_DAMAGE_PER_SECOND);
        }
        duration -= timeStep;
        if (duration < 0) {
            Remove();
            if (particleEmitter != NULL) {
                particleEmitter->Remove();
            }
        }
    }

private:
    ParticleEmitter *particleEmitter;
    float duration = FIRE_DURATION;
};

class FrozenState : public LogicComponent {
URHO3D_OBJECT(FrozenState, LogicComponent);
public:
    explicit FrozenState(Context *context) : LogicComponent(context) {
    }

    void Start() override {
        LogicComponent::Start();
        particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/GreenSmoke.xml");
        particleEmitter->SetEffect(effect);
        auto *entity = GetNode()->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            oldSpeedValue = entity->GetSpeed();
            entity->SetSpeed(oldSpeedValue * FROZEN_SLOW_FACTOR);
        }
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        duration -= timeStep;
        if (duration < 0) {
            auto *entity = GetNode()->GetDerivedComponent<Entity>();
            if (entity != NULL) {
                entity->SetSpeed(oldSpeedValue);
            }
            Remove();
            if (particleEmitter != NULL) {
                particleEmitter->Remove();
            }
        }
    }

private:
    ParticleEmitter *particleEmitter;
    float duration = FROZEN_DURATION;
    float oldSpeedValue;
};

class HealingState : public LogicComponent {
URHO3D_OBJECT(HealingState, LogicComponent);
public:
    explicit HealingState(Context *context) : LogicComponent(context) {
    }

    void Start() override {
        LogicComponent::Start();
        particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/GreenSmoke.xml");
        particleEmitter->SetEffect(effect);
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        auto *entity = GetNode()->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            entity->Heal(timeStep * HEALING_PER_SECOND);
        }
        duration -= timeStep;
        if (duration < 0) {
            Remove();
            if (particleEmitter != NULL) {
                particleEmitter->Remove();
            }
        }
    }

private:
    ParticleEmitter *particleEmitter;
    float duration = HEALING_DURATION;
};

class StrengthState: public LogicComponent {
URHO3D_OBJECT(StrengthState, LogicComponent);
public:
    explicit StrengthState(Context *context) : LogicComponent(context) {}


    void Start() override {
        LogicComponent::Start();
        particleEmitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/GreenSmoke.xml");
        particleEmitter->SetEffect(effect);
        auto *entity = GetNode()->GetDerivedComponent<Entity>();
        if (entity != NULL) {
            oldPowerValue = entity->GetPower();
            entity->SetPower(oldPowerValue * POWER_FACTOR);
        }
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        duration -= timeStep;
        if (duration < 0) {
            auto *entity = GetNode()->GetDerivedComponent<Entity>();
            if (entity != NULL) {
                entity->SetPower(oldPowerValue);
            }
            Remove();
            if (particleEmitter != NULL) {
                particleEmitter->Remove();
            }
        }
    }

private:
    ParticleEmitter *particleEmitter;
    float duration = POWER_STATE_DURATION;
    float oldPowerValue;
};
