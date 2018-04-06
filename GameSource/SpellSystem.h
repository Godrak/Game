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

    SpellBase *createSpell(ShapeNode shape, Node *spellNode) {
        SpellBase *result = NULL;
        switch (shape.shape) {
            case UNKNOWN: {
                auto *eos = spellNode->CreateComponent<EndOfSpell>();
                eos->SetSpellNodeToRemove(spellNode);
                result = eos;
            }
                break;
            case RECTANGLE:
                result = spellNode->CreateComponent<Explosion>();
                break;
            case CIRCLE:
                result = spellNode->CreateComponent<Projectile>();
                break;
            case TRIANGLE:
                result = spellNode->CreateComponent<CasterTarget>();
                break;
            case WATER:
                result = spellNode->CreateComponent<Explosion>();
                break;
            case LIGHTNING:
                result = spellNode->CreateComponent<WallSpell>();
                break;
            case WAVY:
                result = spellNode->CreateComponent<Projectile>();
                break;
        }

        Effect *additionalEffect = NULL;
        switch (shape.shapePattern) {
            case UNKNOWN:
                break;
            case RECTANGLE:
                additionalEffect = spellNode->CreateComponent<FireEffect>();
                break;
            case CIRCLE:
                additionalEffect = spellNode->CreateComponent<HealingEffect>();
                break;
            case TRIANGLE:
                additionalEffect = spellNode->CreateComponent<ShieldEffect>();
                break;
            case WATER:
                additionalEffect = spellNode->CreateComponent<FireEffect>();
                break;
            case LIGHTNING:
                additionalEffect = spellNode->CreateComponent<FrozenEffect>();
                break;
            case WAVY:
                additionalEffect = spellNode->CreateComponent<FireEffect>();
                break;
        }
        if (result != NULL) {
            result->SetAdditionalEffect(additionalEffect);

            for (int i = 0; i < shape.childNodes.size(); ++i) {
                SpellBase *nextSpell = createSpell(shape.childNodes[i], spellNode);
                result->SetNextSpell(nextSpell);
            }
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
