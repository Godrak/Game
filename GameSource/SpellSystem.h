#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include "Spell.h"
#include "Totems.h"
#include <iostream>
#include <memory>

using namespace Urho3D;
using namespace ImageAnalyzer;

class SpellSystem : public LogicComponent {
    const ShapeIndex square{0};
    const ShapeIndex circle{1};
    const ShapeIndex triangle{2};
    const ShapeIndex drop{3};

public:
    explicit SpellSystem(Context *context) : LogicComponent(context) {
        using namespace std;
        ImageAnalyzer::RegisterShapeDescriptor(square, std::unique_ptr<SquareDescriptor>(new SquareDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(circle, std::unique_ptr<CircleDescriptor>(new CircleDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(triangle, std::unique_ptr<TriangleDescriptor>(new TriangleDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(drop, std::unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor()));

        ImageAnalyzer::LoadNetwork("../network.net");
    }

    Totem *CreateTotem(ShapeNode shape, Node *spellNode) {
        Totem *totem = spellNode->CreateComponent<Totem>();
        ShapeIndex index = shape.shape;
        if (index == circle) {
            totem->AddEffect(spellNode->CreateComponent<ShieldEffect>());
        } else if (index == square) {
            totem->AddEffect(spellNode->CreateComponent<FireEffect>());
        } else if (index == triangle) {
            totem->AddEffect(spellNode->CreateComponent<FrozenEffect>());
        } else if (index == drop) {
            totem->AddEffect(spellNode->CreateComponent<HealingEffect>());
        } else {
            //TODO random effect
        }

        if (!shape.childNodes.empty()) {
            CreateAdditionalEffects(shape.childNodes[0], spellNode, totem);
        }
        return totem;
    }

    void CreateAdditionalEffects(ShapeNode shape, Node *spellNode, Totem *totem) {
        ShapeIndex index = shape.shape;
        if (index == circle) {
            totem->AddEffect(spellNode->CreateComponent<PowerEffect>());
        } else if (index == square) {
            totem->AddEffect(spellNode->CreateComponent<StrengthEffect>());
        } else if (index == triangle) {
            totem->AddEffect(spellNode->CreateComponent<DurabilityEffect>());
        } else if (index == drop) {
            totem->AddEffect(spellNode->CreateComponent<DistractionEffect>());
        }
    }

    void CastSpell(Node *caster, vector<Line> lines, Vector3 position) {
        if (lines.empty()) return;

        auto result = ImageAnalyzer::Analyze(ImageLines{lines});
        auto *spellNode = GetScene()->CreateTemporaryChild();
        Totem *totem = CreateTotem(result, spellNode);
        totem->Activate(position);

//        auto *totem = spellNode->CreateComponent<StrengthTotem>();
//        totem->Activate(position);

    }
};
