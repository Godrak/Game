#include <iostream>
#include "UrhoIncludeAll.h"

#pragma once

using namespace Urho3D;

class HitPointsComponent : public LogicComponent {
URHO3D_OBJECT(HitPointsComponent, LogicComponent);
public:
    HitPointsComponent(Context *context) : LogicComponent(context) {}

    void Start() override {
        LogicComponent::Start();
        auto *cache = GetSubsystem<ResourceCache>();

        healthBar = GetScene()->CreateChild();
        healthBar->SetWorldRotation(Quaternion(0, 0, 90));
        auto *staticModel = healthBar->CreateComponent<StaticModel>();
        staticModel->SetModel(cache->GetResource<Model>("Models/Cylinder.mdl"));
        staticModel->SetMaterial(cache->GetResource<Material>("Materials/GreenTransparent.xml"));
    }

    void SetHealth(int maxHealth, int currentHealth = 0) {
        this->maxHealth = maxHealth;
        if (currentHealth == 0) {
            currentHealth = maxHealth;
        }
        this->currentHealth = currentHealth;
    }

    bool ChangeHealth(int health) {
        currentHealth += health;
        currentHealth = std::max(currentHealth, 0);
        currentHealth = std::min(currentHealth, maxHealth);

        return currentHealth > 0;
    }

    int GetHealth() {
        return currentHealth;
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        healthBar->SetWorldPosition(GetNode()->GetPosition() + Vector3::UP * 2);
        healthBar->SetScale(Vector3(0.1, currentHealth / (float) maxHealth, 0.1));
    }

    ~HitPointsComponent() override {
        healthBar->Remove();
    }

private:
    int maxHealth;
    int currentHealth;
    Node *healthBar;

};
