#pragma once

#include "NeuralNetwork.h"
#include "Randomizer.h"

static const int IMAGE_SIDE_SIZE = 32;
static Randomizer RANDOMIZER{};
static const NeuralNetwork NETWORK{"../network.net"};
static const bool DEBUG_IMAGE_SAVE = true;
static const float LINE_DRAWING_STEP_SIZE = 0.5;
