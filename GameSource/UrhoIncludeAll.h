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
static const Urho3D::String PLAYER_NODE_NAME = "Player";
static const Urho3D::String MUTANT_NODE_NAME = "Mutant";

static const bool REMOVE_USED_CURVES = true;

static const int MUTANT_COUNT = 30;

//CAMERA
static const float CAMERA_Y_AXIS_OFFSET = 0.3f;
static const int MAX_VERTICAL_DISTANCE = 80;
static const int MIN_VERTICAL_DISTANCE = 8;

//DRAWING
static const float DRAWING_SAMPLE_STEP_SIZE = 0.2f;
static const float DRAWING_SAMPLE_TIME = 0.05f;

//FIRE
static const float FIRE_DAMAGE_PER_SECOND = 15;
static const float FIRE_DURATION = 6;

//HEALING
static const float HEALING_PER_SECOND = 35;
static const float HEALING_DURATION = 6;

//FROZEN
static const float FROZEN_DURATION = 5;
static const float FROZEN_SLOW_FACTOR = 0.4f;

//POWER_STATE
static const float POWER_STATE_DURATION = 5;
static const float POWER_FACTOR = 5;

//SHIELD
static const float SHIELD_POWER_CHARGE = 3;
static const float MAX_SHIELD_POWER = 100;

//DURABILITY
static const float DURABILITY_EXTENSION = 20;

//REACH
static const float REACH_EXTENSION = 6;

//WALL_EFFECT
static const float WALL_RADIUS = 9;

//TOTEMS
static const float TOTEM_SCALE = 2;
static const float TOTEM_DURATION = 20;
static const float TOTEM_EFFECT_TRIGGER_TIME = 1;
static const float TOTEM_EFFECT_TRIGGER_WINDOW = 0.05;
static const float TOTEM_EFFECT_SIZE = 7;

//PROJECTILE
static const float PROJECTILE_MIN_LINEAR_VELOCITY_LENGTH = 3;
static const float PROJECTILE_SCALE = 0.1f;
static const float PROJECTILE_STRENGTH = 20;
static const float PROJECTILE_MASS = 2;

//EXPLOSION
static const float EXPLOSION_SCALE = 1.f;
static const float EXPLOSION_STRENGTH_MULTIPLIER = 0.7f;
static const float EXPLOSION_DAMAGE_MULTIPLIER = 5.f;
static const float EXPLOSION_TIME_LIMIT = 0.3f;

//MUTANT
static const float MUTANT_HEALTH = 200;
static const float MUTANT_SPEED = 0.3f;
static const float MUTANT_ATTACK_DURATION = 1.3f;
static const float MUTANT_ATTACK_DAMAGE = 15.f;