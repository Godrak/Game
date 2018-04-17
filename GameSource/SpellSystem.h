#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"
#include "FireComponent.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include "Spell.h"
#include <iostream>
#include <memory>

using namespace Urho3D;
using namespace ImageAnalyzer;

class SpellSystem : public LogicComponent {
    const ShapeIndex square{0};
    const ShapeIndex circle{1};
    const ShapeIndex triangle{2};
    const ShapeIndex cross{3};

public:
    explicit SpellSystem(Context *context) : LogicComponent(context) {
        using namespace std;
        ImageAnalyzer::RegisterShapeDescriptor(square, std::unique_ptr<SquareDescriptor>(new SquareDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(circle, std::unique_ptr<CircleDescriptor>(new CircleDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(triangle, std::unique_ptr<TriangleDescriptor>(new TriangleDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(cross, std::unique_ptr<CrossDescriptor>(new CrossDescriptor()));

        ImageAnalyzer::LoadNetwork("../network.net");
    }

    SpellBase *createSpell(ShapeNode shape, Node *spellNode) {
        SpellBase *result = NULL;
        ShapeIndex index = shape.shape;
        if (index == circle) {
            result = spellNode->CreateComponent<Projectile>();
        } else if (index == square) {
            result = spellNode->CreateComponent<Explosion>();
        } else if (index == triangle) {
            result = spellNode->CreateComponent<WallSpell>();
        } else if (index == cross) {
            result = spellNode->CreateComponent<LocalSpell>();
        } else {
            auto *eos = spellNode->CreateComponent<EndOfSpell>();
            eos->SetSpellNodeToRemove(spellNode);
            result = eos;
        }

        index = shape.shapePattern;
        if (index == circle) {
            result->SetAdditionalEffect(spellNode->CreateComponent<FireEffect>());
        } else if (index == square) {
            result->SetAdditionalEffect(spellNode->CreateComponent<FrozenEffect>());
        } else if (index == triangle) {
            result->SetAdditionalEffect(spellNode->CreateComponent<FireEffect>());
        } else if (index == cross) {
            result->SetAdditionalEffect(spellNode->CreateComponent<HealingEffect>());
        }

        if (!shape.childNodes.empty()) {
            SpellBase *nextSpell = createSpell(shape.childNodes[0], spellNode);
            result->SetNextSpell(nextSpell);
        }
        return result;
    }

    void castSpell(Node *caster, vector<Line> lines, Vector3 position) {
        auto result = ImageAnalyzer::Analyze(ImageLines{lines});
        auto *spellNode = GetScene()->CreateTemporaryChild();
        SpellBase *spell = createSpell(result, spellNode);
        spell->ActivateSpell(position, caster);
    }
};
