#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "../ImageAnalyzer/ExampleShapeDescriptors.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include "Spell.h"
#include "Totems.h"
#include <iostream>
#include <atomic>
#include <utility>
#include <future>

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

        ImageAnalyzer::COMPOSED_SHAPES_ENABLED = true;
        ImageAnalyzer::EMBEDDED_SHAPES_ENABLED = true;
        ImageAnalyzer::ROTATIONS_ENABLED = true;
        ImageAnalyzer::ROTATION_SAMPLES_COUNT = 15;
        ImageAnalyzer::COMPOSITION_SAMPLES_LIMIT = 5;
        ImageAnalyzer::SHAPE_VALUE_LIMIT = 0.5;

        ImageAnalyzer::DEBUG_IMAGE_SAVE = false;

        ImageAnalyzer::RegisterShapeDescriptor(square, std::unique_ptr<SquareDescriptor>(new SquareDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(circle, std::unique_ptr<CircleDescriptor>(new CircleDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(triangle, std::unique_ptr<TriangleDescriptor>(new TriangleDescriptor()));
        ImageAnalyzer::RegisterShapeDescriptor(drop, std::unique_ptr<WaterDropDescriptor>(new WaterDropDescriptor()));

        ImageAnalyzer::LoadNetwork("../network.net");
    }

    Totem *CreateTotem(ShapeNode shape, Node *spellNode) {
        auto *totem = spellNode->CreateComponent<Totem>();
        ShapeIndex index = shape.shape;
        if (index == circle) {
            totem->AddEffect(spellNode->CreateComponent<ShieldEffect>());
        } else if (index == square) {
            totem->AddEffect(spellNode->CreateComponent<FireEffect>());
        } else if (index == triangle) {
            totem->AddEffect(spellNode->CreateComponent<FrozenEffect>());
        } else if (index == drop) {
            totem->AddEffect(spellNode->CreateComponent<HealingEffect>());
        } else totem->ExtendDuration(-TOTEM_DURATION);

        CreateAdvancedEffects(shape.shapePattern, spellNode, totem);

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

    void CreateAdvancedEffects(ShapeIndex index, Node* spellNode, Totem *totem){
        if (index == circle) {
            totem->AddEffect(spellNode->CreateComponent<DefenseWallEffect>());
        } else if (index == square) {
            totem->AddEffect(spellNode->CreateComponent<ExplosionsEffect>());
        } else if (index == triangle) {
            totem->AddEffect(spellNode->CreateComponent<TowerEffect>());
        } else if (index == drop) {
            totem->AddEffect(spellNode->CreateComponent<MadnessEffect>());
        }
    }
};


// core_task class represents each unique task, that is, it cannot be
// copied and user doesn't have to know about its existence.
// core_task class doesn't need void specialization.
template<class Ret>
class core_task {
public:
    core_task() {
        has_finished.store(false);
    }

    core_task(const core_task &orig) = delete;

    void start(std::function<Ret(ImageLines)> f, ImageLines image) {
        std::lock_guard<std::mutex> start_guard(start_lock);

        std::bind(f, image);

        future = std::async(std::launch::async, [this, image](ImageLines image) {
            auto ret = ImageAnalyzer::Analyze(image);
            this->done();
            return ret;
        }, image);
    }

    Ret get_result() {
        std::lock_guard<std::mutex> result_guard(result_lock);
        return future.get();
    }

    bool is_done() {
        std::lock_guard<std::mutex> result_guard(result_lock);
        return has_finished.load();
    }

    void done() {
        std::lock_guard<std::mutex> result_guard(result_lock);
        has_finished.store(true);
    }

private:
    std::mutex result_lock;
    std::mutex start_lock;
    std::atomic<bool> has_finished;
    std::shared_future<Ret> future;

};


class Spell : public LogicComponent {
URHO3D_OBJECT(Spell, LogicComponent);

    Spell(Context *context) : LogicComponent(context) {}

    void Start(vector<Line> lines, Vector3 position) {
        ImageLines image(lines);
        analysis.start(ImageAnalyzer::Analyze, image);
        GetNode()->SetWorldPosition(position);

        emitter = GetNode()->CreateComponent<ParticleEmitter>();
        auto effect = GetSubsystem<ResourceCache>()->GetResource<ParticleEffect>("Data/Particle/GreenFire.xml");
        emitter->SetEffect(effect);
    };

    void Update(float timeStep) override {
        if (!finished && analysis.is_done()) {
            finished = true;
            CastSpell();
            emitter->Remove();
            this->Remove();
        }
    }

    void CastSpell() {
        auto shapeNode = analysis.get_result();
        auto* totem = GetScene()->GetComponent<SpellSystem>()->CreateTotem(shapeNode, GetNode());
        totem->Activate(GetNode()->GetPosition());
    };

private:
    ParticleEmitter *emitter;
    core_task<ShapeNode> analysis{};
    bool finished = false;
};
