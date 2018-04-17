#pragma once

#include "GrayScaleImage.h"
#include "ImageAnalyzer.h"

using namespace ImageAnalyzer;

namespace LineDrawer {

    void RandomizeData(GrayScaleImage &image, Randomizer &randomizer);

    void DrawLine(const Line &line, GrayScaleImage &image, int imageSize, float drawingStepSize);

    GrayScaleImage DrawLines(const ImageLines &imageLines, int imageSize, float drawingStepSize);

};
