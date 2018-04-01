#pragma once

#include "GrayScaleImage.h"
#include "ImageAnalyzer.h"
#include "Params.h"

using namespace ImageAnalyzer;

namespace LineDrawer {

    void RandomizeData(GrayScaleImage &image);

    void DrawLine(const Line &line, GrayScaleImage &image);

    GrayScaleImage DrawLines(const ImageLines &imageLines);


};
