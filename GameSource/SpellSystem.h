#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/Params.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "FireComponent.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include "Spell.h"
#include <iostream>

using namespace Urho3D;
using namespace ImageAnalyzer;

class SpellSystem : public LogicComponent {
public:
    explicit SpellSystem(Context *context) : LogicComponent(context) {}

    void castSpell(Node *caster, vector<Line> lines) {
        auto result = ImageAnalyzer::Analyze(ImageLines{lines});


        auto *spell = GetScene()->CreateChild();
        auto *projectile = spell->CreateComponent<Projectile>();
        projectile->SetUp(Vector3::UP * 10, caster->GetPosition() + Vector3::UP * 3);

    }
};
