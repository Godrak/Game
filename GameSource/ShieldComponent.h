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
    }

    bool ChangeShieldPower(int power) {
        currentShieldPower += power;
        if (currentShieldPower < 0) {
            Remove();
        }
        return currentShieldPower <= 0;
    }

    int GetPower() {
        return currentShieldPower;
    }

    void Update(float timeStep) override {
        LogicComponent::Update(timeStep);
        shieldSphere->SetWorldPosition(GetNode()->GetPosition() + Vector3::UP);
        material->SetShaderParameter("MatDiffColor", Color(0.2, 0.2, 0.8, min(currentShieldPower / 10.0f, 0.3)));
    }

    ~ShieldComponent() override {
        shieldSphere->Remove();
    }

private:
    float min(float a, float b) {
        return a < b ? a : b;
    }

    int currentShieldPower;
    Node *shieldSphere;
    Material *material;

};
