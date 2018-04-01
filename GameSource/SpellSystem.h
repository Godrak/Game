#pragma once

#include "UrhoIncludeAll.h"
#include "../ImageAnalyzer/GrayScaleImage.h"
#include "../ImageAnalyzer/Params.h"
#include "../ImageAnalyzer/ImageAnalyzer.h"
#include "FireBurstComponent.h"
#include "TimeOutComponent.h"
#include "DrawableTexture.h"
#include <iostream>

using namespace Urho3D;
using namespace ImageAnalyzer;

class SpellSystem : public LogicComponent {
public:
    explicit SpellSystem(Context *context) : LogicComponent(context) {}

    void castSpell(Node *caster, vector<Line> lines) {
        ImageAnalyzer::Analyze(ImageLines{lines});
    }

//    void CreateFire(Vector3 position, Shape type, Vector3 scale = Vector3::ONE) {
//        auto *fireNode = GetScene()->CreateChild("Fire");
//        fireNode->SetPosition(position);
//        fireNode->SetScale(scale);
//
//        switch (type) {
//            case RECTANGLE:
//                fireNode->CreateComponent<BlueFireBurstComponent>();
//                break;
//            case CIRCLE:
//                fireNode->CreateComponent<FireBurstComponent>();
//                break;
//            case CROSS:
//                fireNode->CreateComponent<GreenFireBurstComponent>();
//                break;
//            case DIAGONAL_CROSS:
//                fireNode->CreateComponent<PurpleFireBurstComponent>();
//                break;
//            case UNKNOWN:
//                break;
//        }
//
//        auto *timeOutComponent = fireNode->CreateComponent<TimeOutComponent>();
//        timeOutComponent->SetTimeToLive(3);
//    }

};
