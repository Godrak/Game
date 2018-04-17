#pragma once

#include "UrhoIncludeAll.h"
#include "TimeOutComponent.h"

using namespace Urho3D;

Node *SpawnSound(const Node *node, const Vector3 &pos, const String &soundName, float duration) {
    auto *cache = node->GetSubsystem<ResourceCache>();
    Node *newNode = node->GetScene()->CreateChild("Sound");
    newNode->SetPosition(pos);

    // Create the sound source
    auto *source = newNode->CreateComponent<SoundSource3D>();
    auto *sound = cache->GetResource<Sound>(soundName);
    source->SetDistanceAttenuation(200, 5000, 1);
    source->Play(sound);

    auto *toc = newNode->CreateComponent<TimeOutComponent>();
    toc->SetTimeToLive(duration);

    return newNode;
}