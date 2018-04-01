#include <iostream>
#include "UrhoIncludeAll.h"

#pragma once

using namespace Urho3D;

class TimeOutComponent : public LogicComponent {
URHO3D_OBJECT(TimeOutComponent, LogicComponent);
public:
    explicit TimeOutComponent(Context *context) : LogicComponent(context) {
    }

    void SetTimeToLive(float seconds){
        timeToLiveInSeconds = seconds;
    }

    void FixedUpdate(float timeStep) override {
        LogicComponent::FixedUpdate(timeStep);

        if (timeToLiveInSeconds > 0){
            timeToLiveInSeconds -= timeStep;
        } else{
            GetNode()->Remove();
        }
    }

private:
    float timeToLiveInSeconds;
};
