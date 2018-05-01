#include <iostream>
#include "UrhoIncludeAll.h"

#pragma once

using namespace Urho3D;

class ShieldComponent : public LogicComponent {
URHO3D_OBJECT(ShieldComponent, LogicComponent);
public:
    ShieldComponent(Context *context) : LogicComponent(context) {}

    void Start() override {
        LogicComponent::Start();
        auto *cache = GetSubsystem<ResourceCache>();

        shieldSphere = GetScene()->CreateChild();
        shieldSphere->SetScale(GetNode()->GetWorldScale() * Vector3(1.6, 2.3, 1.6));
        auto *staticModel = shieldSphere->CreateComponent<StaticModel>();
        staticModel->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
        material = cache->GetResource<Material>("Materials/BlueTransparent.xml");
        staticModel->SetMaterial(material);

        shieldBar = GetScene()->CreateChild();
        shieldBar->SetWorldRotation(Quaternion(0, 0, 90));
        staticModel = shieldBar->CreateComponent<StaticModel>();
        staticModel->SetModel(cache->GetResource<Model>("Models/Cylinder.mdl"));
        staticModel->SetMaterial(cache->GetResource<Material>("Materials/BlueTransparent.xml"));
    }

    bool ChangeShieldPower(float power) {
        currentShieldPower += power;
        currentShieldPower = std::max(currentShieldPower, 0.f);
        currentShieldPower = std::min(currentShieldPower, shieldPower);
        return currentShieldPower <= 0;
    }

    float GetPower() {
        return currentShieldPower;
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        shieldSphere->SetWorldPosition(GetNode()->GetPosition() + Vector3::UP);
        material->SetShaderParameter("MatDiffColor", Color(0.2, 0.2, 0.8, min(currentShieldPower / 50.0f, 0.5)));
        shieldSphere->GetComponent<StaticModel>()->SetMaterial(material);
        shieldBar->SetWorldPosition(GetNode()->GetPosition() + Vector3::UP * 2 + Vector3::UP * 0.3);
        shieldBar->SetScale(Vector3(0.1, currentShieldPower / shieldPower, 0.1));


        if (currentShieldPower <= 0) {
            Remove();
        }
    }

    ~ShieldComponent() override {
        shieldSphere->Remove();
    }

private:
    float min(float a, float b) {
        return a < b ? a : b;
    }

    float shieldPower = MAX_SHIELD_POWER;
    float currentShieldPower;
    Node *shieldSphere;
    Node *shieldBar;
    Material *material;

};
