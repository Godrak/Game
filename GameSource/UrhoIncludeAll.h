#pragma once

#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Audio/AudioEvents.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/DebugNew.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/DrawableEvents.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Animatable.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text3D.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Urho2D/Drawable2D.h>
#include <Urho3D/Urho3D.h>

//CONSTANTS

//CAMERA
static const float CAMERA_Y_AXIS_OFFSET = 0.3f;
static const int MAX_VERTICAL_DISTANCE = 25;
static const int MIN_VERTICAL_DISTANCE = 8;

//DRAWING
static const float DRAWING_SAMPLE_STEP_SIZE = 0.2f;
static const float DRAWING_SAMPLE_TIME = 0.05f;

//FIRE
static const float FIRE_DAMAGE_PER_SECOND = 2;
static const float FIRE_DURATION = 6;

//HEALING
static const float HEALING_PER_SECOND = 3;
static const float HEALING_DURATION = 5;

//SHIELD_EFFECT
static const int SHIELD_POWER = 50;

//PROJECTILE
static const float PROJECTILE_MIN_LINEAR_VELOCITY_LENGTH = 7;
static const float PROJECTILE_SCALE = 0.7f;
static const float PROJECTILE_STRENGTH = 10;

//EXPLOSION
static const float EXPLOSION_SCALE = 6.f;
static const float EXPLOSION_STRENGTH_MULTIPLIER = 20.f;
static const float EXPLOSION_TIME_LIMIT = 0.3f;

//MUTANT
static const float MUTANT_HEALTH = 200;